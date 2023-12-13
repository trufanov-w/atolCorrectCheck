#include "atol.h"
#include <QDebug>

Atol::Atol(QObject *parent) :
    QObject(parent),
    m_pDriver(nullptr)
{
  libfptr_create(&m_pDriver);
  m_printCheck = true;
  m_printCorrCheckReturn = true;
  m_printCorrCheck = true;
  m_printCorrCheckReturnFirst = true;
  m_printAdditional = false;
}


Atol::~Atol() {}

bool Atol::connect() {

  libfptr_set_single_setting(m_pDriver, LIBFPTR_SETTING_MODEL, stringToAtol(QString::number(LIBFPTR_MODEL_ATOL_AUTO)));
  libfptr_set_single_setting(m_pDriver, LIBFPTR_SETTING_PORT, stringToAtol(QString::number(LIBFPTR_PORT_USB)));
  libfptr_apply_single_settings(m_pDriver);

  qDebug() << "ATOL: " << "Open connection";

  bool result = libfptr_open(m_pDriver) == 0;

  qDebug() << "ATOL: " << "Connect result" << result;

  if (!result) registerError();
  logErrors();

  return isConnected();
}

bool Atol::isConnected() {
  return libfptr_is_opened(m_pDriver) != 0;
}

bool Atol::disconnect() {
  if (isConnected()) {
    return libfptr_close(m_pDriver) == 0;
  }
}

void Atol::startCorrentDocument(const int &docNo) {
  resetDocuments();
  if (readDocument(docNo)) {
    createCorrectDocuments();
  }
}

void Atol::setPrintCheck(bool print) {
  m_printCheck = print;
}

void Atol::printCorrCheckReturn(bool value) {
  m_printCorrCheckReturn = value;
}

void Atol::printCorrCheck(bool value) {
  m_printCorrCheck = value;
}

void Atol::printCorrCheckReturnFirst(bool value) {
  m_printCorrCheckReturnFirst = value;
}

void Atol::printAdditional(bool value) {
  m_printAdditional = value;
}

void Atol::resetDocuments() {
  m_sellMap.clear();
  m_sellCorrectionMap.clear();
  m_sellReturnCorrectionMap.clear();
}

bool Atol::readDocument(const int &docNo) {
  QJsonDocument jsonDocument;
  QJsonObject obj;
  obj.insert("type", "getFnDocument");
  obj.insert("fiscalDocumentNumber", docNo);
  jsonDocument.setObject(obj);

  if (!execute(jsonDocument.toVariant().toMap())) {
    return false;
  }

  QString str = QString::fromStdWString(&m_result[0]).toUtf8();  
  qInfo() << "-----------------------------------------------------------------------------------";
  qInfo() << "### прочитанный документ!!!!";
  qInfo() << str;
  qInfo() << "-----------------------------------------------------------------------------------";
  m_sellMap = QJsonDocument::fromJson(str.toUtf8()).toVariant().toMap();

  return true;
}

void Atol::createCorrectDocuments() {
  QVariantMap map = m_sellMap;

  QVariantMap tlv = map["documentTLV"].toMap();

  if (tlv["1054"].toInt() != 1) {
    resetDocuments();
    return;
  }

  QDateTime dt = tlv["1012"].toDateTime();

  QVariantList items = tlv["1059"].toList();

  loginOperator(tlv["1021"].toString(), tlv["1018"].toString());

  QJsonDocument jsonCorrect;
  QJsonObject objCorrect;

  QVariantMap correct;
  QVariantMap returnCorrect;
  QVariantMap kassir;
  QVariantList itemsCorrect;
  QVariantMap item;
  QVariantMap tax;
  QVariantMap payment;
  QVariantMap additional;
  QVariantList payments;


  additional["type"] = "additionalAttribute";
  additional["value"] = tlv["1040"].toString();
  additional["print"] = m_printAdditional;

  correct["type"] = "sellCorrection";
  correct["taxationType"] = getTaxationType(tlv["1055"].toInt());
  correct["electronically"] = !m_printCheck;
  correct["ignoreNonFiscalPrintErrors"] = false;
  correct["correctionType"] = "self";
  correct["correctionBaseDate"] = tlv["1012"].toDateTime().toString("yyyy.MM.dd");
  correct["correctionBaseNumber"] = "0";

  for (int i = 0; i < items.size(); i++) {
    item.clear();
    tax.clear();
    item["type"] = "position";
    item["name"] = items[i].toMap()["1030"].toString();tlv["1040"].toString();
    item["price"] = items[i].toMap()["1079"].toDouble();
    item["quantity"] = items[i].toMap()["1023"].toDouble();
    item["amount"] = items[i].toMap()["1043"].toDouble();
    item["measurementUnit"] = "шт.";

    tax["type"] = getTaxType(items[i].toMap()["1199"].toInt());

    item["tax"] = tax;

    itemsCorrect.append(item);
  }
  itemsCorrect.append(additional);
  correct["items"] = itemsCorrect;


  double summ = tlv["1031"].toDouble();
  if (summ > 0) {
    payment["type"] = "cash";
    payment["sum"] = tlv["1031"].toDouble();
    payments.append(payment);
  }

  summ = tlv["1081"].toDouble();
  if (summ > 0) {
    payment["type"] = "electronically";
    payment["sum"] = tlv["1031"].toDouble();
    payments.append(payment);
  }

  correct["payments"] = payments;
  correct["total"] = tlv["1020"].toDouble();

  returnCorrect = correct;
  returnCorrect["type"] = "sellReturnCorrection";

  m_sellCorrectionMap = correct;
  m_sellReturnCorrectionMap = returnCorrect;


  if (m_printCorrCheckReturn && m_printCorrCheckReturnFirst && execute(m_sellReturnCorrectionMap)) {
    QString str = QString::fromStdWString(&m_result[0]).toUtf8();
    qInfo() << "-----------------------------------------------------------------------------------";
    qInfo() << "### ответ на создание чека коррекции возврата прихода";
    qInfo() << str;
    qInfo() << "-----------------------------------------------------------------------------------";
  }

  if (m_printCorrCheck && execute(m_sellCorrectionMap)) {
    QString str = QString::fromStdWString(&m_result[0]).toUtf8();
    qInfo() << "-----------------------------------------------------------------------------------";
    qInfo() << "### ответ на создание чека коррекции прихода";
    qInfo() << str;
    qInfo() << "-----------------------------------------------------------------------------------";
  }

  if (m_printCorrCheckReturn && !m_printCorrCheckReturnFirst && execute(m_sellReturnCorrectionMap)) {
    QString str = QString::fromStdWString(&m_result[0]).toUtf8();
    qInfo() << "-----------------------------------------------------------------------------------";
    qInfo() << "### ответ на создание чека коррекции возврата прихода";
    qInfo() << str;
    qInfo() << "-----------------------------------------------------------------------------------";
  }
}

bool Atol::execute(QVariantMap data) {
  QString str = QJsonDocument::fromVariant(data).toJson(QJsonDocument::Compact);
  qInfo() << "-----------------------------------------------------------------------------------";
  qInfo() << "### >> ЗАПРОС К УСТРОЙСТВУ!!!";
  qInfo() << str;
  qInfo() << "-----------------------------------------------------------------------------------";
  m_wstdString = str.toStdWString();

  libfptr_set_param_str(m_pDriver, LIBFPTR_PARAM_JSON_DATA, m_wstdString.c_str());

  if (libfptr_process_json(m_pDriver) != LIBFPTR_OK) {
    return false;
  }

  std::vector<wchar_t> result(128);
  int size = libfptr_get_param_str(m_pDriver, LIBFPTR_PARAM_JSON_DATA, &result[0], result.size());
  if (size == 0) {
    return false;
  }
  if (size > result.size()) {
    result.resize(size);
    libfptr_get_param_str(m_pDriver, LIBFPTR_PARAM_JSON_DATA, &result[0], result.size());
  }
  m_result = result;
  return true;
}

const wchar_t *Atol::stringToAtol(const QString &str) {
  int size = str.left(STRING_SIZE).toWCharArray(m_stdString);
  m_stdString[size] = 0;
  return m_stdString;
}

void Atol::clearErrors() {
  m_errors.clear();
}

void Atol::registerError() {
  ErrorInfo info;
  std::vector<wchar_t> description(32);

  info.code = libfptr_error_code(m_pDriver);
  int size = libfptr_error_description(m_pDriver, &description[0], static_cast<int>(description.size()));
  if (size > static_cast<int>(description.size())) {
    description.resize(static_cast<size_t>(size));
    libfptr_error_description(m_pDriver, &description[0], static_cast<int>(description.size()));
  }
  info.description = QString::fromWCharArray(&description[0], size);
  m_errors.append(info);
}

void Atol::logErrors() {
  if (!m_errors.size()) return;

  for (int i = 0; i < m_errors.size(); i++)
    qDebug() << "ATOL Error:" << m_errors[i].code << m_errors[i].description;
}

QVector<uchar> Atol::readByteArrayAttribute(int paramNumber) {
  QVector<uchar> value(STRING_SIZE);
  int size = libfptr_get_param_bytearray(m_pDriver, paramNumber, &value[0], value.size());

  if (size > value.size()) {
    value.resize(size);
    libfptr_get_param_bytearray(m_pDriver, paramNumber, &value[0], value.size());
  }
  value.resize(size);
  return value;
}

QString Atol::getTaxationType(int tax) {
  switch (tax) {
  case 1:
    return "osn";
    break;
  case 2:
    return "usnIncome";
    break;
  case 4:
    return "usnIncomeOutcome";
    break;
  case 16:
    return "esn";
    break;
  case 32:
    return "patent";
    break;
  default:
    return "osn";
    break;
  }

}

QString Atol::getTaxType(int tax) {
  switch (tax) {
  case 0:
    return "none";
    break;
  case 2:
    return "vat10";
    break;
  case 4:
    return "vat110";
    break;
  case 5:
    return "vat0";
    break;
  case 6:
    return "none";
    break;
  case 7:
    return "vat20";
    break;
  case 8:
    return "vat120";
    break;
  default:
    return "none";
    break;
  }
}

bool Atol::loginOperator(const QString &cashierName, const QString &cashierInn) {
  qDebug() << "ATOL: " << "Login operator" << cashierName << ":" << cashierInn;
  libfptr_set_param_str(m_pDriver, 1021, stringToAtol(cashierName));
  if (cashierInn.length() > 10) {
    libfptr_set_param_str(m_pDriver, 1203, stringToAtol(cashierInn)); // для ИП передавать ИНН кассира не обязательно (ТЕГ 1203)
  } else {
    libfptr_set_param_str(m_pDriver, 1018, stringToAtol(cashierInn)); // для ООО передавать ИНН обязательно (ТЕГ 1018)
  }
  bool result = libfptr_operator_login(m_pDriver) == 0;
  if (!result) registerError();
  return result;
}

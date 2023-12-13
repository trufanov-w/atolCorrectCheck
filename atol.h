#ifndef ATOL_H
#define ATOL_H

#include <QObject>
#include <QDateTime>
#include <QVariantMap>
#include <QJsonObject>
#include <QJsonDocument>
#include "atol/include/libfptr10.h"

class Atol : public QObject {
  Q_OBJECT
  public:
  explicit Atol(QObject *parent = nullptr);
  ~Atol();

  bool connect();
  bool isConnected();
  bool disconnect();

  void startCorrentDocument(const int &docNo);

  void setPrintCheck(bool print);
  void printCorrCheckReturn(bool value);
  void printCorrCheck(bool value);
  void printCorrCheckReturnFirst(bool value);
  void printAdditional(bool value);
  void printTeg1192(bool value);

  private:
  void resetDocuments();
  bool readDocument(const int &docNo);
  void createCorrectDocuments();

  bool execute(QVariantMap data);

  bool m_printCorrCheckReturn;
  bool m_printCorrCheck;
  bool m_printCorrCheckReturnFirst;
  bool m_printAdditional;
  bool m_printTeg1192;

  std::vector<wchar_t> m_result;

  bool m_printCheck;

  QVariantMap m_sellMap;
  QVariantMap m_sellCorrectionMap;
  QVariantMap m_sellReturnCorrectionMap;


  struct ErrorInfo {
    int code;
    QString description;
  };

  libfptr_handle m_pDriver;

  QList<ErrorInfo> m_errors;

  static const int STRING_SIZE = 1024;
  wchar_t m_stdString[STRING_SIZE + 1];
  std::wstring m_wstdString;

  const wchar_t *stringToAtol(const QString &str);

  void clearErrors();
  void registerError();
  void logErrors();

  QVector<uchar> readByteArrayAttribute(int paramNumber);

  QString getTaxationType(int tax);
  QString getTaxType(int tax);


  bool loginOperator(const QString &cashierName, const QString &cashierInn);
};

#endif // ATOL_H

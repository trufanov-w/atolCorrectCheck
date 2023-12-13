#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "atol.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  atol = new Atol(this);
}

MainWindow::~MainWindow()
{
  delete ui;
}


void MainWindow::on_btnConnect_clicked() {
  QString style = "QPushButton#btnConnect { background-color: %1}";
  QString title = "Подключено";

  if (atol->isConnected()) {
    atol->disconnect();
  } else {
    atol->connect();
  }

  if (atol->isConnected()) {
    style = style.arg("green");
    title= "Подключено";
  } else {
    style = style.arg("red");
    title= "Отключено";
  }
  ui->btnConnect->setStyleSheet(style);
  ui->btnConnect->setText(title);
}

void MainWindow::on_btnCorrectCheck_clicked() {
  docNo = ui->sbFirstCheck->value();
  ui->lblCurrentDocument->setText(QString::number(docNo));
  atol->startCorrentDocument(docNo);
}

void MainWindow::on_btnCorrectNext_clicked() {
  if (docNo == 0) return;
  docNo++;
  ui->lblCurrentDocument->setText(QString::number(docNo));
  atol->startCorrentDocument(docNo);
}

void MainWindow::on_cbPrintCheck_stateChanged(int arg1) {
  atol->setPrintCheck(ui->cbPrintCheck->isChecked());
}


void MainWindow::on_cbPrintCorrReturnCheck_stateChanged(int arg1) {
  atol->printCorrCheckReturn(ui->cbPrintCorrReturnCheck->isChecked());
}


void MainWindow::on_cbPrintCorrCheck_stateChanged(int arg1) {
  atol->printCorrCheck(ui->cbPrintCorrCheck->isChecked());
}


void MainWindow::on_cbPriorityReturnCorrCheck_stateChanged(int arg1) {
  atol->printCorrCheckReturnFirst(ui->cbPriorityReturnCorrCheck->isChecked());
}


void MainWindow::on_cbPrintAdditional_stateChanged(int arg1) {
  atol->printAdditional(ui->cbPrintAdditional->isChecked());
}


void MainWindow::on_cbTag1192_stateChanged(int arg1) {
    atol->printTeg1192(ui->cbTag1192->isChecked());
}


void MainWindow::on_leCorrect_textChanged(const QString &arg1) {
    atol->correctText(arg1);
}


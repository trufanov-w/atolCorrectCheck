#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "atol.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
  Q_OBJECT

  public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

  private slots:
  void on_btnConnect_clicked();

  void on_btnCorrectCheck_clicked();

  void on_btnCorrectNext_clicked();

  void on_cbPrintCheck_stateChanged(int arg1);

  void on_cbPrintCorrReturnCheck_stateChanged(int arg1);

  void on_cbPrintCorrCheck_stateChanged(int arg1);

  void on_cbPriorityReturnCorrCheck_stateChanged(int arg1);

  void on_cbPrintAdditional_stateChanged(int arg1);

  void on_cbTag1192_stateChanged(int arg1);

  void on_leCorrect_textChanged(const QString &arg1);

  private:
  Ui::MainWindow *ui;
  Atol *atol;
  int docNo;
};
#endif // MAINWINDOW_H

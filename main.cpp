#include "logger.h"
#include "mainwindow.h"

#include <QApplication>

static Logger *pLogger = nullptr;

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
  pLogger->logOutput(type, context, msg);
}

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  pLogger = new Logger();

  qInstallMessageHandler(myMessageOutput);

  qInfo() << ":start:";
  MainWindow w;
  w.show();
  return a.exec();
}

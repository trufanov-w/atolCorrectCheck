#ifndef LOGGER_H
#define LOGGER_H

#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QTimer>

class Logger : public QObject
{
  Q_OBJECT
  public:
  explicit Logger(QObject *parent = nullptr);
  ~Logger();
  void setLogDirectory(const QString& directory);

  public slots:
  void logOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);

  private:
  QFile m_currentLogFile;
  qint64 m_fileSizeLimit;
  qint64 m_currentFileSize;
  int m_currentFileNumber;
  QString m_logFile;
  QString m_logDirectory;

  void rotateLogFileIfNeeded();
  QString getNextLogFileName();
  void writeLogToFile(const QString& message);
};


#endif // LOGGER_H

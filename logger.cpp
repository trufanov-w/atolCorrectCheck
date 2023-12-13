#include "logger.h"
#include <QDir>

Logger::Logger(QObject *parent) :
    QObject(parent)
{
  m_fileSizeLimit = 5 * 1024 * 1024; // 5 MB
  m_logFile = "posAtol";
  m_currentFileSize = 0;
  m_currentFileNumber = 0;
  m_logDirectory = QDir::currentPath() + "/logs";

  if (!QDir(m_logDirectory).exists()) {
    QDir().mkdir(m_logDirectory);
  }
}

Logger::~Logger() {}

void Logger::setLogDirectory(const QString &directory) {
  m_logDirectory = directory;
  if (!QDir(m_logDirectory).exists()) {
    QDir().mkdir(m_logDirectory);
  }
}

void Logger::logOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
  QString logMessage = QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss.zzz] ");
  switch (type) {
  case QtDebugMsg:
    logMessage += QString("DEBUG: %1").arg(msg);
    break;
  case QtWarningMsg:
    logMessage += QString("WARNING: %1").arg(msg);
    break;
  case QtCriticalMsg:
    logMessage += QString("CRITICAL: %1").arg(msg);
    break;
  case QtFatalMsg:
    logMessage += QString("FATAL: %1").arg(msg);
    break;
  case QtInfoMsg:
    logMessage += QString("INFO: %1").arg(msg);
    break;
  }

  writeLogToFile(logMessage);
}

void Logger::rotateLogFileIfNeeded() {
  if (m_currentFileSize > m_fileSizeLimit) {
    m_currentLogFile.close();
    m_currentFileSize = 0;
  }
}

QString Logger::getNextLogFileName() {
  if (!m_currentLogFile.isOpen() || m_currentFileSize > m_fileSizeLimit) {
    m_currentLogFile.close();
    m_currentFileNumber++;
    QString newFileName = QString("%1/%2_%3.log").arg(m_logDirectory).arg(m_logFile).arg(m_currentFileNumber);
    m_currentLogFile.setFileName(newFileName);
    m_currentLogFile.open(QIODevice::ReadWrite | QIODevice::Append);
    m_currentFileSize = 0;
  }
  return m_currentLogFile.fileName();
}

void Logger::writeLogToFile(const QString &message) {
  QFile file(getNextLogFileName());
  if (file.open(QIODevice::ReadWrite | QIODevice::Append)) {
    QTextStream out(&file);
    out << message << endl;
    m_currentFileSize = file.size();
    file.close();
  }
  rotateLogFileIfNeeded();
}

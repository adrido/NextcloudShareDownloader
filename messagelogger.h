#ifndef MESSAGELOGGER_H
#define MESSAGELOGGER_H

#include <QObject>
#include <utility>

class MessageLogger : public QObject
{
    Q_OBJECT
public:
    enum class LogLevel{Info, Warning, Error, Folder, File};
    MessageLogger();
    void log(LogLevel logLevel, QString msg, QString filename = QString());
    void info(QString msg){
        log(LogLevel::Info, std::move(msg));
    }
    void warning(QString msg) {
        log(LogLevel::Warning, std::move(msg));
    }
    void error(QString msg){
        log(LogLevel::Error, std::move(msg));
    }
    void file(const QString& file){
        log(LogLevel::File, file, file);
    }

signals:
    void signalLogMsg(MessageLogger::LogLevel logLevel, QString msg);
};

#endif // MESSAGELOGGER_H

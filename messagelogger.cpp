#include "messagelogger.h"

MessageLogger::MessageLogger()
{

}

void MessageLogger::log(MessageLogger::LogLevel logLevel, QString msg, QString file)
{
    Q_UNUSED(file)
    emit signalLogMsg(logLevel, msg);
}

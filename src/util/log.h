#pragma once
#include <QDebug>
#include <string>

class Logger
{
public:
    enum Level { Error = 0, Warn = 1, Info = 2, Debug = 3 };

    static void setLevel(Level level) { s_level = level; }
    static Level level() { return s_level; }
    static void setLevelFromString(const std::string& s);

private:
    static Level s_level;
};

#define LOG_DEBUG if (Logger::level() >= Logger::Debug) qDebug()
#define LOG_INFO  if (Logger::level() >= Logger::Info)  qInfo()
#define LOG_WARN  if (Logger::level() >= Logger::Warn)  qWarning()
#define LOG_ERROR if (Logger::level() >= Logger::Error) qCritical()

#include "log.h"

Logger::Level Logger::s_level = Logger::Warn;

void Logger::setLevelFromString(const std::string& s)
{
    if      (s == "debug") setLevel(Debug);
    else if (s == "info")  setLevel(Info);
    else if (s == "warn")  setLevel(Warn);
    else if (s == "error") setLevel(Error);
}

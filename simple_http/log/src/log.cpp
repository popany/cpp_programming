#include "log.h"

Logger::Logger():
    logLevel(LEVEL_ALWAYS)
{
    fs.open("./log.txt");
    if (!fs.is_open()) {
        std::cout << "failed to open logfile" << std::endl;
        exit(1);
    }
}

Logger::~Logger()
{
    fs.close();
}

void Logger::SetLogLevel(std::string s)
{
    if (s == "error") {
        logLevel = LEVEL_ERROR;
        LogAlways("SetLogLevel: error");
    } else if (s == "warn") {
        logLevel = LEVEL_WARN;
        LogAlways("SetLogLevel: warn");
    } else if (s == "info") {
        logLevel = LEVEL_INFO;
        LogAlways("SetLogLevel: info");
    } else if (s == "debug") {
        logLevel = LEVEL_DEBUG;
        LogAlways("SetLogLevel: debug");
    }
}

Logger& Logger::GetInstance()
{
    static Logger instance;
    return instance;
}

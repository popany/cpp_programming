#pragma once
#include "spdlog/spdlog.h"

#define LOG_DEBUG(...) do { \
    if (spdlog::get_level() <= SPDLOG_LEVEL_DEBUG) { \
        SPDLOG_DEBUG(__VA_ARGS__); \
    } \
} while (0)

#define LOG_INFO(...) SPDLOG_INFO(__VA_ARGS__)
#define LOG_WARN(...) SPDLOG_WARN(__VA_ARGS__)
#define LOG_ERROR(...) SPDLOG_ERROR(__VA_ARGS__)

enum LogLevel 
{
    DEBUG = 1,
    INFO = 2,
    WARN = 3,
    ERROR = 4,
};

inline LogLevel parseLogLevel(std::string logLevel)
{
    if (logLevel == "DEBUG") {
        return LogLevel::DEBUG;
    }
    if (logLevel == "INFO") {
        return LogLevel::INFO;
    }
    if (logLevel == "WARN") {
        return LogLevel::WARN;
    }
    if (logLevel == "ERROR") {
        return LogLevel::ERROR;
    }
    return LogLevel::INFO;
}

void InitLogger();
void SetLogLevel(LogLevel);

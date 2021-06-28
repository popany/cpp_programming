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
    LOG_DEBUG = 1,
    LOG_INFO = 2,
    LOG_WARN = 3,
    LOG_ERROR = 4,
};

void InitLogger(std::string logPath);
void SetLogLevel(LogLevel);

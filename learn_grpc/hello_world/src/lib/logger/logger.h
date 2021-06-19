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

enum LOG_LEVEL
{
    DEBUG = 1,
    INFO = 2,
    WARN = 3,
    ERROR = 4,
};

void InitLogger();
void SetLogLevel(LOG_LEVEL);

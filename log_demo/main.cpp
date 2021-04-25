#include "logger.h"

#include <iostream>

int main()
{
    InitLogger();
    SetLogLevel("debug");
    LOG_DEBUG("debug");
    LOG_INFO("info: {}", "2");
    LOG_WARN("warn: {}", "3");
    LOG_ERROR("error: {}", "4");

    return 0;
}

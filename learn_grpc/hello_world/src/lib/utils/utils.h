#pragma once

#include <string>
#include "logger.h"

namespace utils
{
    std::string GetCurrentTimeString(const std::string& fmt = "%Y-%m-%d %H:%M:%S.%f");
    LogLevel ParseLogLevel(std::string logLevel);
    bool StringToBool(const std::string& s);
}

#pragma once

#include <string>
#include <sstream>
#include <iomanip>
#include "logger.h"

namespace utils
{
    std::string GetCurrentTimeString(const std::string& fmt = "%Y-%m-%d %H:%M:%S.%f");
    LogLevel ParseLogLevel(std::string logLevel);
    bool StringToBool(const std::string& s);

    template<typename T>
    std::string IntToHex(T i)
    {
        std::stringstream ss;
        ss << "0x" 
            << std::setfill('0') << std::setw(sizeof(T)*2) 
            << std::hex << i;

        return ss.str();
    }

    inline std::string PtrToHex(void* p)
    {
        std::stringstream ss;
        ss << "0x" 
            << std::setfill('0') << std::setw(sizeof(p)*2) 
            << std::hex << (size_t)p;

        return ss.str();
    }

}

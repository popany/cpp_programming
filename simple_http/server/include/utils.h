#pragma once
#include <string>
#include <cctype>
#include <errno.h>
#include <string.h>
#include <algorithm>

inline std::string GetErrorMsg(std::string info, int errorCode)
{
    return info + ", error code(" + std::to_string(errorCode) + "), " + strerror(errorCode);
}

inline void Ltrim(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) { return !std::isspace(ch); }));
}

inline void Rtrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) { return !std::isspace(ch); }).base(), s.end());
}

inline void Trim(std::string &s)
{
    Ltrim(s);
    Rtrim(s);
}

#pragma once
#include <string>
#include <cctype>
#include <errno.h>
#include <string.h>
#include <algorithm>
#include <condition_variable>
#include <mutex>

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

class EventNotifier
{
    bool signaled;
    std::condition_variable cv;
    std::mutex m;

public:
    EventNotifier():
        signaled(false)
    {}

    void Set()
    {
        {
            std::lock_guard<std::mutex> lk(m);
            signaled = true;
        }
        cv.notify_one();
    }

    void Wait()
    {
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, [&] { return signaled; });
        signaled = false;
    }
};

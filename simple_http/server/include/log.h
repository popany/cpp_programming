#pragma once
#include <iostream>
#include <fstream>
#include <mutex>

class Logger
{
    enum {
        LEVEL_ERROR = 0,
        LEVEL_WARN,
        LEVEL_INFO,
        LEVEL_DEBUG,
        LEVEL_ALWAYS
    };

    int logLevel;
    std::ofstream fs;
    std::mutex m;

    Logger();

    template <typename T>
    void Log(T t) 
    {
        fs << t << "\n";
        std::cout << t << "\n";
    }

    template<typename T, typename... Args>
    void Log(T t, Args... args)
    {
        fs << t;
        std::cout << t;
        Log(args...) ;
    }

public:
    Logger(const Logger&) = delete;
    void operator=(const Logger&) = delete;

    static Logger& GetInstance();

    ~Logger();

    void SetLogLevel(std::string s);

    template<typename... Args>
    void LogAlways(Args... args)
    {
        std::lock_guard<std::mutex> lk(m);
        Log(args...);
    }

    template<typename... Args>
    void LogInfo(Args... args)
    {
        if (logLevel < LEVEL_INFO) {
            return;
        }

        std::lock_guard<std::mutex> lk(m);
        Log("[Info]", args...);
    }

    template<typename... Args>
    void LogWarn(Args... args)
    {
        if (logLevel < LEVEL_WARN) {
            return;
        }

        std::lock_guard<std::mutex> lk(m);
        Log("[Warn]", args...);
    }

    template<typename... Args>
    void LogError(Args... args)
    {
        if (logLevel < LEVEL_ERROR) {
            return;
        }

        std::lock_guard<std::mutex> lk(m);
        Log("[Error]", args...);
    }

    template<typename... Args>
    void LogDebug(Args... args)
    {
        if (logLevel < LEVEL_DEBUG) {
            return;
        }

        std::lock_guard<std::mutex> lk(m);
        Log("[Debug]", args...);
    }
};

#define LogInfo(...) Logger::GetInstance().LogInfo(std::string("[")+__PRETTY_FUNCTION__+"] ",__VA_ARGS__)
#define LogDebug(...) Logger::GetInstance().LogDebug(std::string("[")+__PRETTY_FUNCTION__+"] ",__VA_ARGS__)
#define LogWarn(...) Logger::GetInstance().LogWarn(std::string("[")+__PRETTY_FUNCTION__+"] ",__VA_ARGS__)
#define LogError(...) Logger::GetInstance().LogError(std::string("[")+__PRETTY_FUNCTION__+"] ",__VA_ARGS__)
#define LogAlways(...) Logger::GetInstance().LogAlways(std::string("[")+__PRETTY_FUNCTION__+"] ",__VA_ARGS__)

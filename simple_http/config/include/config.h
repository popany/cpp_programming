#pragma once
#include <string>

class Config
{
    enum {
        DEFAULT_PORT = 5666,
        EP_EVENT_COUNT = 10,
    };

    void ParseLine(std::string s);

    Config();

public:

    std::string logLevel;
    uint16_t port;
    size_t epEventCount;

    Config(const Config&) = delete;
    void operator=(const Config&) = delete;

    static Config& GetInstance();
};

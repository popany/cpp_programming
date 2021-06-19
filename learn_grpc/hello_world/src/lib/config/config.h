#pragma once

#include <map>
#include "logger.h"

#define LOG_LEVEL "log.level"
#define GRPC_SERVER_PORT "grpc.server.port"

#define DEFINE_CONFIG_ITEM(item,type,parser,default)\
    type _##item;\
    void init_##item()\
    {\
        _##item = default;\
    }\
    void set_##item(const std::string& k, const std::string& v)\
    {\
        if (k != name) {\
            return;\
        }\
        _##item = parser(v);\
    }\
    public:\
    const type& _##item() const\
    {\
        return _##item;\
    }\
    private:

#define INIT_CONFIG(item) do {\
        init_##item();
    } while (0)

#define SET_CONFIG(item,k,v) do {\
        set_##item(k, v);
    } while (0)

#define CONFIG config.getInstance()

class Config
{
    LOG_LEVEL parseLogLevel(std::string logLevel)
    {
        if (logLevel == "DEBUG") {
            return LOG_LEVEL::DEBUG;
        }
        if (logLevel == "INFO") {
            return LOG_LEVEL::INFO;
        }
        if (logLevel == "WARN") {
            return LOG_LEVEL::WARN;
        }
        if (logLevel == "ERROR") {
            return LOG_LEVEL::ERROR;
        }
        return LOG_LEVEL::INFO;
    }

    DEFINE_CONFIG_ITEM(LOG_LEVEL, std::string, parseLogLevel, "INFO");
    DEFINE_CONFIG_ITEM(GRPC_SERVER_PORT, std::string, std::stoi, 50051);

    void initConfig()
    {
        INIT_CONFIG(LOG_LEVEL);
        INIT_CONFIG(GRPC_SERVER_PORT);
    }

    void setConfig(const std::string& k, const std::string& v)
    {
        SET_CONFIG(LOG_LEVEL);
        SET_CONFIG(GRPC_SERVER_PORT);
    }

    void loadConfigFile();
    Config()
public:
    Config(const Config&) = delete;
    void operator=(const Config&) = delete;

    static Config& getInstance();
};

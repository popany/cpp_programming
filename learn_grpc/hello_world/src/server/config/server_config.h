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
        if (k != item) {\
            return;\
        }\
        _##item = parser(v);\
    }\
    public:\
    const type& GET_##item() const\
    {\
        return _##item;\
    }\
    private:

#define INIT_CONFIG(item) do {\
        init_##item();\
    } while (0)

#define SET_CONFIG(item,k,v) do {\
        set_##item(k, v);\
    } while (0)

#define SERVER_CONFIG ServerConfig.getInstance()

class ServerConfig
{
    LogLevel parseLogLevel(std::string logLevel)
    {
        if (logLevel == "DEBUG") {
            return LogLevel::DEBUG;
        }
        if (logLevel == "INFO") {
            return LogLevel::INFO;
        }
        if (logLevel == "WARN") {
            return LogLevel::WARN;
        }
        if (logLevel == "ERROR") {
            return LogLevel::ERROR;
        }
        return LogLevel::INFO;
    }

    DEFINE_CONFIG_ITEM(LOG_LEVEL, std::string, parseLogLevel, "INFO");
    DEFINE_CONFIG_ITEM(GRPC_SERVER_PORT, std::string, std::stoi, "50051");

    void initConfig()
    {
        INIT_CONFIG(LOG_LEVEL);
        INIT_CONFIG(GRPC_SERVER_PORT);
    }

    void setConfig(const std::string& name, const std::string& value)
    {
        SET_CONFIG(LOG_LEVEL, name, value);
        SET_CONFIG(GRPC_SERVER_PORT, name, value);
    }

    void loadConfigFile();
    ServerConfig();
public:
    ServerConfig(const ServerConfig&) = delete;
    void operator=(const ServerConfig&) = delete;

    static ServerConfig& getInstance();
};

#undef LOG_LEVEL
#undef GRPC_SERVER_PORT
#undef DEFINE_CONFIG_ITEM
#undef INIT_CONFIG
#undef SET_CONFIG
#undef SERVER_CONFIG

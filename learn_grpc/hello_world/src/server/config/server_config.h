#pragma once

#include "logger.h"
#include "abstract_config.h"

const std::string SERVER_CONFIG_FILE_NAME = "server.config";
#define LOG_LEVEL "log.level"
#define GRPC_SERVER_PORT "grpc.server.port"

class ServerConfig : public AbstractConfig
{
    DEFINE_CONFIG_ITEM(LOG_LEVEL, LogLevel, parseLogLevel, "INFO");
    DEFINE_CONFIG_ITEM(GRPC_SERVER_PORT, int, std::stoi, "50051");

    void initConfig()
    {
        INIT_CONFIG(LOG_LEVEL);
        INIT_CONFIG(GRPC_SERVER_PORT);
    }

    void setConfig(const std::string& name, const std::string& value)
    {
        if (SET_CONFIG(LOG_LEVEL, name, value)) {
            SetLogLevel(GET_LOG_LEVEL());
        }
        SET_CONFIG(GRPC_SERVER_PORT, name, value);
    }

    const std::string& getConfigFileName() override
    {
        return SERVER_CONFIG_FILE_NAME;
    }

    ServerConfig() {}
public:
    ServerConfig(const ServerConfig&) = delete;
    void operator=(const ServerConfig&) = delete;

    void init()
    {
        initConfig();
        loadConfigFile();
    }

    static ServerConfig& getInstance()
    {
        static ServerConfig config;
        return config;
    }
};

#define SERVER_CONFIG (ServerConfig::getInstance())

#undef LOG_LEVEL
#undef GRPC_SERVER_PORT

#undef DEFINE_CONFIG_ITEM
#undef INIT_CONFIG
#undef SET_CONFIG

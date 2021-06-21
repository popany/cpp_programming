#pragma once

#include "logger.h"
#include "abstract_config.h"

const std::string DEFAULT_CLIENT_CONFIG_FILE_PATH = "./client.config";
#define LOG_LEVEL "log.level"
#define GRPC_SERVER_PORT "grpc.server.port"

class ClientConfig : public AbstractConfig
{
    DEFINE_CONFIG_ITEM(LOG_LEVEL, LogLevel, parseLogLevel, "INFO");
    DEFINE_CONFIG_ITEM(GRPC_SERVER_PORT, int, std::stoi, "50051");

    void initConfig() override
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

    ClientConfig() {}

    void init()
    {
        AbstractConfig::init(DEFAULT_CLIENT_CONFIG_FILE_PATH);
    }

public:
    ClientConfig(const ClientConfig&) = delete;
    void operator=(const ClientConfig&) = delete;

    static ClientConfig& getInstance()
    {
        static ClientConfig config;
        static int _init = (config.init(), 0);
        return config;
    }
};

#define CLIENT_CONFIG (ClientConfig::getInstance())

#undef LOG_LEVEL
#undef GRPC_SERVER_PORT

#undef DEFINE_CONFIG_ITEM
#undef INIT_CONFIG
#undef SET_CONFIG

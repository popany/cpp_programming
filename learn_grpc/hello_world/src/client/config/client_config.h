#pragma once

#include "logger.h"
#include "utils.h"
#include "abstract_config.h"

const std::string DEFAULT_CLIENT_CONFIG_FILE_PATH = "./client.config";
#define LOG_LEVEL "log.level"
#define GRPC_SERVER_HOST "grpc.server.host"
#define GRPC_SERVER_PORT "grpc.server.port"
#define GRPC_CLIENT_ASYNC "grpc.client.async"
#define GRPC_CLIENT_ASYNC_REQUEST_COUNT "grpc.client.async.request.count"

class ClientConfig : public AbstractConfig
{
    DEFINE_CONFIG_ITEM(LOG_LEVEL, LogLevel, utils::ParseLogLevel, "INFO");
    DEFINE_CONFIG_ITEM(GRPC_SERVER_HOST, std::string, std::string, "localhost");
    DEFINE_CONFIG_ITEM(GRPC_SERVER_PORT, int, std::stoi, "50051");
    DEFINE_CONFIG_ITEM(GRPC_CLIENT_ASYNC, bool, utils::StringToBool, "false");
    DEFINE_CONFIG_ITEM(GRPC_CLIENT_ASYNC_REQUEST_COUNT, int, std::stoi, "3");

    void initConfig() override
    {
        INIT_CONFIG(LOG_LEVEL);
        INIT_CONFIG(GRPC_SERVER_HOST);
        INIT_CONFIG(GRPC_SERVER_PORT);
        INIT_CONFIG(GRPC_CLIENT_ASYNC);
        INIT_CONFIG(GRPC_CLIENT_ASYNC_REQUEST_COUNT);
    }

    void setConfig(const std::string& name, const std::string& value)
    {
        if (SET_CONFIG(LOG_LEVEL, name, value)) {
            SetLogLevel(GET_LOG_LEVEL());
        }
        SET_CONFIG(GRPC_SERVER_HOST, name, value);
        SET_CONFIG(GRPC_SERVER_PORT, name, value);
        SET_CONFIG(GRPC_CLIENT_ASYNC, name, value);
        SET_CONFIG(GRPC_CLIENT_ASYNC_REQUEST_COUNT, name, value);
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

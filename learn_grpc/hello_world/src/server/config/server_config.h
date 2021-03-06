#pragma once

#include "logger.h"
#include "utils.h"
#include "abstract_config.h"

const std::string DEFAULT_SERVER_CONFIG_FILE_PATH = "./server.config";
#define LOG_LEVEL "log.level"
#define GRPC_SERVER_PORT "grpc.server.port"
#define GRPC_SERVER_ASYNC "grpc.server.async"
#define GRPC_SERVER_ASYNC_THREADPOOL_SIZE "grpc.server.async.threadpool.size"
#define GRPC_SERVER_ASYNC_POLLING_INTERVAL_MILLISECONDS "grpc.server.async.polling.interval.milliseconds"

class ServerConfig : public AbstractConfig
{
    DEFINE_CONFIG_ITEM(LOG_LEVEL, LogLevel, utils::ParseLogLevel, "INFO");
    DEFINE_CONFIG_ITEM(GRPC_SERVER_PORT, int, std::stoi, "50051");
    DEFINE_CONFIG_ITEM(GRPC_SERVER_ASYNC, bool, utils::StringToBool, "false");
    DEFINE_CONFIG_ITEM(GRPC_SERVER_ASYNC_THREADPOOL_SIZE, int, std::stoi, "8");
    DEFINE_CONFIG_ITEM(GRPC_SERVER_ASYNC_POLLING_INTERVAL_MILLISECONDS, int, std::stoi, "500");

    void initConfig() override
    {
        INIT_CONFIG(LOG_LEVEL);
        INIT_CONFIG(GRPC_SERVER_PORT);
        INIT_CONFIG(GRPC_SERVER_ASYNC);
        INIT_CONFIG(GRPC_SERVER_ASYNC_THREADPOOL_SIZE);
        INIT_CONFIG(GRPC_SERVER_ASYNC_POLLING_INTERVAL_MILLISECONDS);
    }

    void setConfig(const std::string& name, const std::string& value)
    {
        if (SET_CONFIG(LOG_LEVEL, name, value)) {
            SetLogLevel(GET_LOG_LEVEL());
        }
        SET_CONFIG(GRPC_SERVER_PORT, name, value);
        SET_CONFIG(GRPC_SERVER_ASYNC, name, value);
        SET_CONFIG(GRPC_SERVER_ASYNC_THREADPOOL_SIZE, name, value);
        SET_CONFIG(GRPC_SERVER_ASYNC_POLLING_INTERVAL_MILLISECONDS, name, value);
    }

    ServerConfig() {}

    void init()
    {
        AbstractConfig::init(DEFAULT_SERVER_CONFIG_FILE_PATH);
    }

public:
    ServerConfig(const ServerConfig&) = delete;
    void operator=(const ServerConfig&) = delete;

    static ServerConfig& getInstance()
    {
        static ServerConfig config;
        static int _init = (config.init(), 0);
        return config;
    }
};

#define SERVER_CONFIG (ServerConfig::getInstance())

#undef LOG_LEVEL
#undef GRPC_SERVER_PORT

#undef DEFINE_CONFIG_ITEM
#undef INIT_CONFIG
#undef SET_CONFIG

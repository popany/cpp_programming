#include "logger.h"
#include "config/server_config.h"

int main()
{
    SERVER_CONFIG.init();
    LOG_INFO("iiii");
    LOG_DEBUG("dddd");

    LOG_INFO("log.level: {}", SERVER_CONFIG.GET_LOG_LEVEL());
    LOG_INFO("grpc.server.port: {}", SERVER_CONFIG.GET_GRPC_SERVER_PORT());

    return 0;
}

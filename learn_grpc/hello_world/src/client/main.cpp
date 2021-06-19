#include "logger.h"
#include "config/client_config.h"

int main()
{
    CLIENT_CONFIG.init();
    LOG_INFO("iiii");
    LOG_DEBUG("dddd");

    LOG_INFO("log.level: {}", CLIENT_CONFIG.GET_LOG_LEVEL());
    LOG_INFO("grpc.server.port: {}", CLIENT_CONFIG.GET_GRPC_SERVER_PORT());

    return 0;
}
#include "logger.h"
#include "config/client_config.h"
#include "client.h"
#include <grpcpp/grpcpp.h>

int main()
{
    InitLogger();
    SetLogLevel(CLIENT_CONFIG.GET_LOG_LEVEL());

    std::string serverAddress = std::string("0.0.0.0:") + std::to_string(CLIENT_CONFIG.GET_GRPC_SERVER_PORT());
    HelloClient client(grpc::CreateChannel(serverAddress, grpc::InsecureChannelCredentials()));
    client.sayHello();
    client.sayHelloAgain();

    return 0;
}
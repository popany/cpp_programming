#include "server.h"
#include <grpcpp/grpcpp.h>
#include "hello_service.h"
#include "goodbye_service.h"
#include "chat_service.h"
#include "logger.h"
#include "config/server_config.h"

HelloService::Service& GetHelloService()
{
    return HelloServiceImpl::getInstance();
}

GoodbyeService::Service& GetGoodbyeService()
{
    return GoodbyeServiceImpl::getInstance();
}

ChatService::Service& GetChatService()
{
    return ChatServiceImpl::getInstance();
}

Server::Server()
{}

Server& Server::getInstance()
{
    static Server instance;
    return instance;
}

void Server::start()
{
    std::string serverAddress = std::string("0.0.0.0:") + std::to_string(SERVER_CONFIG.GET_GRPC_SERVER_PORT());
    LOG_INFO("Server listening on {}", serverAddress);
    grpc::ServerBuilder builder;
    builder.AddListeningPort(serverAddress, grpc::InsecureServerCredentials());
    builder.RegisterService(&GetHelloService());
    builder.RegisterService(&GetGoodbyeService());
    builder.RegisterService(&GetChatService());
    grpcServer = builder.BuildAndStart();
    grpcServer->Wait();
}

void Server::stop()
{
    if (grpcServer) {
        grpcServer->Shutdown();
    }
    LOG_INFO("Server stopped");
}

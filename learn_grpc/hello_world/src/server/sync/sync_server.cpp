#include "sync_server.h"
#include <grpcpp/grpcpp.h>
#include "hello_service.h"
#include "goodbye_service.h"
#include "chat_service.h"
#include "logger.h"
#include "../config/server_config.h"

static HelloService::Service& GetHelloService()
{
    return HelloServiceImpl::getInstance();
}

static GoodbyeService::Service& GetGoodbyeService()
{
    return GoodbyeServiceImpl::getInstance();
}

static ChatService::Service& GetChatService()
{
    return ChatServiceImpl::getInstance();
}

SyncServer::SyncServer()
{}

SyncServer& SyncServer::getInstance()
{
    static SyncServer instance;
    return instance;
}

void SyncServer::start()
{
    std::string serverAddress = std::string("0.0.0.0:") + std::to_string(SERVER_CONFIG.GET_GRPC_SERVER_PORT());
    LOG_INFO("SyncServer listening on {}", serverAddress);
    grpc::ServerBuilder builder;
    builder.AddListeningPort(serverAddress, grpc::InsecureServerCredentials());
    builder.RegisterService(&GetHelloService());
    builder.RegisterService(&GetGoodbyeService());
    builder.RegisterService(&GetChatService());
    grpcServer = builder.BuildAndStart();
    grpcServer->Wait();
}

void SyncServer::stop()
{
    if (grpcServer) {
        grpcServer->Shutdown();
    }
    LOG_INFO("SyncServer stopped");
}

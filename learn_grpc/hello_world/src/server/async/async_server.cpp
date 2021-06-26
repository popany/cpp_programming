#include "async_server.h"
#include <grpcpp/grpcpp.h>
#include "../config/server_config.h"
#include "server_proactor.h"

AsyncServer::AsyncServer()
{}

AsyncServer& AsyncServer::getInstance()
{
    static AsyncServer instance;
    return instance;
}

void AsyncServer::start()
{
    std::string serverAddress = std::string("0.0.0.0:") + std::to_string(SERVER_CONFIG.GET_GRPC_SERVER_PORT());
    LOG_INFO("SyncServer listening on {}", serverAddress);

    grpc::ServerBuilder builder;
    builder.AddListeningPort(serverAddress, grpc::InsecureServerCredentials());
    builder.RegisterService(&ServerProactor::getInstance().getHelloService());
    builder.RegisterService(&ServerProactor::getInstance().getGoodbyeService());
    builder.RegisterService(&ServerProactor::getInstance().getChatService());

    for (int i = 0; i < ServerProactor::getInstance().getThreadPoolSize(); i++) {
        ServerProactor::getInstance().addCompletionQueue(builder.AddCompletionQueue());
    }

    grpcServer = builder.BuildAndStart();
    ServerProactor::getInstance().startDemuxer();
    grpcServer->Wait();
    ServerProactor::getInstance().waitForComplete();
}

void AsyncServer::stop()
{
    if (grpcServer) {
        grpcServer->Shutdown();
    }
    LOG_INFO("SyncServer stopped");
}

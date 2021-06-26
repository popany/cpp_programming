#pragma once

#include <grpcpp/grpcpp.h>
#include <memory>

class SyncServer
{
    std::unique_ptr<grpc::Server> grpcServer;
    SyncServer();
public:
    SyncServer(const SyncServer&) = delete;
    void operator=(const SyncServer&) = delete;
    void start();
    void stop();
    static SyncServer& getInstance();
};

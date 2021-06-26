#pragma once

#include <memory>
#include <grpcpp/grpcpp.h>

class AsyncServer
{
    std::unique_ptr<grpc::Server> grpcServer;
    AsyncServer();
public:
    AsyncServer(const AsyncServer&) = delete;
    void operator=(const AsyncServer&) = delete;

    void start();
    void stop();

    static AsyncServer& getInstance();
};

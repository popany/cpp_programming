#pragma once

#include "hello_service.h"
#include <memory>

class Server
{
    std::unique_ptr<grpc::Server> grpcServer;
    Server();
public:
    Server(const Server&) = delete;
    void operator=(const Server&) = delete;
    void start();
    void stop();
    static Server& getInstance();
};

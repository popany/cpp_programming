#pragma once

#include "logger.h"
#include "hello.grpc.pb.h"

class HelloServiceImpl : public HelloService::Service
{
    HelloServiceImpl();
public:
    HelloServiceImpl(const HelloServiceImpl&) = delete;
    void operator=(const HelloServiceImpl&) = delete;

    static HelloServiceImpl& getInstance();
    grpc::Status sayHello(grpc::ServerContext* context, const HelloRequest* request, HelloResponse* response) override;
    grpc::Status sayHelloAgain(grpc::ServerContext* context, const HelloRequest* request, HelloResponse* response) override;
};

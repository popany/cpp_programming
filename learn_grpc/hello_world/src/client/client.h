#pragma once

#include <grpcpp/grpcpp.h>
#include "hello.grpc.pb.h"
#include <memory>

class HelloClient
{
    std::unique_ptr<HelloService::Stub> stub;
public:
    HelloClient(std::shared_ptr<grpc::Channel> channel);
    void sayHello();
    void sayHelloAgain();
};

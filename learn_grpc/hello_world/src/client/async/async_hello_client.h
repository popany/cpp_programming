#pragma once

#include <grpcpp/grpcpp.h>
#include "hello.grpc.pb.h"
#include "client_proactor.h"

class AsyncHelloClient
{
    std::unique_ptr<HelloService::Stub> stub;

    void processSayHelloResponse(void* token);
    void processSayHelloAgainResponse(void* token);

public:
    AsyncHelloClient(std::shared_ptr<grpc::Channel> channel);
    void sayHello();
    void sayHelloAgain();
};

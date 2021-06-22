#pragma once

#include <grpcpp/grpcpp.h>
#include "hello.grpc.pb.h"
#include "abstract_async_client.h"

class AsyncHelloClient : public AbstractAsyncClient 
{
    std::unique_ptr<HelloService::Stub> stub;

    void processSayHelloResponse(void* token);
    void processSayHelloAgainResponse(void* token);

public:
    AsyncHelloClient(std::shared_ptr<grpc::Channel> channel, int threadPoolSize = 8);
    void sayHello();
    void sayHelloAgain();
};

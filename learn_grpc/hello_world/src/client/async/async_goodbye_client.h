#pragma once

#include <grpcpp/grpcpp.h>
#include "goodbye.grpc.pb.h"
#include "abstract_async_client.h"

class AsyncGoodbyeClient : public AbstractAsyncClient 
{
    std::unique_ptr<GoodbyeService::Stub> stub;

    void processSayGoodbyeResponse(void* token);
    void processSayGoodbyeAgainResponse(void* token);

public:
    AsyncGoodbyeClient(std::shared_ptr<grpc::Channel> channel, int threadPoolSize = 8);
    void sayGoodbye();
    void sayGoodbyeAgain();
};

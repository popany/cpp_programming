#pragma once

#include <grpcpp/grpcpp.h>
#include "goodbye.grpc.pb.h"
#include "client_proactor.h"

class AsyncGoodbyeClient
{
    std::unique_ptr<GoodbyeService::Stub> stub;

    void processSayGoodbyeResponse(void* token);
    void processSayGoodbyeAgainResponse(void* token);

public:
    AsyncGoodbyeClient(std::shared_ptr<grpc::Channel> channel);
    void sayGoodbye();
    void sayGoodbyeAgain();
};

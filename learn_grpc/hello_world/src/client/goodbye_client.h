#pragma once

#include <grpcpp/grpcpp.h>
#include "goodbye.grpc.pb.h"
#include <memory>

class GoodbyeClient
{
    std::unique_ptr<GoodbyeService::Stub> stub;
public:
    GoodbyeClient(std::shared_ptr<grpc::Channel> channel);
    void sayGoodbye();
    void sayGoodbyeAgain();
};

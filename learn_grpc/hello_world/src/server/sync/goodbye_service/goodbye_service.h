#pragma once

#include "logger.h"
#include "goodbye.grpc.pb.h"

class GoodbyeServiceImpl : public GoodbyeService::Service
{
    GoodbyeServiceImpl();
public:
    GoodbyeServiceImpl(const GoodbyeServiceImpl&) = delete;
    void operator=(const GoodbyeServiceImpl&) = delete;

    static GoodbyeServiceImpl& getInstance();
    grpc::Status sayGoodbye(grpc::ServerContext* context, const GoodbyeRequest* request, GoodbyeResponse* response) override;
    grpc::Status sayGoodbyeAgain(grpc::ServerContext* context, const GoodbyeRequest* request, GoodbyeResponse* response) override;
};

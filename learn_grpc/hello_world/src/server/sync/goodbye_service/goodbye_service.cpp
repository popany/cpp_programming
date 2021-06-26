#include "goodbye_service.h"

GoodbyeServiceImpl::GoodbyeServiceImpl()
{}

GoodbyeServiceImpl& GoodbyeServiceImpl::getInstance()
{
    static GoodbyeServiceImpl instance;
    return instance;
}

grpc::Status GoodbyeServiceImpl::sayGoodbye(grpc::ServerContext* context, const GoodbyeRequest* request, GoodbyeResponse* response)
{
    LOG_INFO("firstname: {}, lastname: {}", request->firstname(), request->lastname());
    response->set_greeting(std::string("Goodbye, ") + request->firstname() + " " + request->lastname());
    return grpc::Status::OK;
}

grpc::Status GoodbyeServiceImpl::sayGoodbyeAgain(grpc::ServerContext* context, const GoodbyeRequest* request, GoodbyeResponse* response)
{
    LOG_INFO("firstname: {}, lastname: {}", request->firstname(), request->lastname());
    response->set_greeting(std::string("Goodbye again, ") + request->firstname() + " " + request->lastname());
    return grpc::Status::OK;
}

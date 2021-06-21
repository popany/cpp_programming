#include "chat_service.h"
#include "utils.h"

ChatServiceImpl::ChatServiceImpl()
{}

ChatServiceImpl& ChatServiceImpl::getInstance()
{
    static ChatServiceImpl instance;
    return instance;
}

grpc::Status ChatServiceImpl::greet(grpc::ServerContext* context, const ClientWords* request, ServerWords* response)
{
    LOG_INFO("request: {} - \"{}\"", request->timestamp(), request->content());
    response->set_timestamp(GetCurrentTimeString());
    response->set_content("Me too!");
    return grpc::Status::OK;
}

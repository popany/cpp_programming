#include "hello_service.h"

HelloServiceImpl::HelloServiceImpl()
{}

HelloServiceImpl& HelloServiceImpl::getInstance()
{
    static HelloServiceImpl instance;
    return instance;
}

grpc::Status HelloServiceImpl::sayHello(grpc::ServerContext* context, const HelloRequest* request, HelloResponse* response)
{
    LOG_INFO("firstname: {}, lastname: {}", request->firstname(), request->lastname());
    response->set_greeting(std::string("Hello, ") + request->firstname() + " " + request->lastname());
    return grpc::Status::OK;
}

grpc::Status HelloServiceImpl::sayHelloAgain(grpc::ServerContext* context, const HelloRequest* request, HelloResponse* response)
{
    LOG_INFO("firstname: {}, lastname: {}", request->firstname(), request->lastname());
    response->set_greeting(std::string("Hello again, ") + request->firstname() + " " + request->lastname());
    return grpc::Status::OK;
}

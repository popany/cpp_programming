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
    response->set_timestamp(utils::GetCurrentTimeString());
    response->set_content("Me too!");
    return grpc::Status::OK;
}

grpc::Status ChatServiceImpl::listen(grpc::ServerContext* context, const ClientWords* request, grpc::ServerWriter<ServerWords>* writer)
{
    LOG_INFO("request: {} - \"{}\"", request->timestamp(), request->content());
    
    ServerWords response;
    response.set_timestamp(utils::GetCurrentTimeString());
    response.set_content("aaa");
    writer->Write(response);

    response.set_timestamp(utils::GetCurrentTimeString());
    response.set_content("bbb");
    writer->Write(response);

    response.set_timestamp(utils::GetCurrentTimeString());
    response.set_content("ccc");
    writer->Write(response);

    return grpc::Status::OK;
}

grpc::Status ChatServiceImpl::speak(grpc::ServerContext* context, grpc::ServerReader<ClientWords>* reader, ServerWords* response)
{
    ClientWords request;
    while (reader->Read(&request)) {
        LOG_INFO("request: {} - \"{}\"", request.timestamp(), request.content());
    }

    response->set_timestamp(utils::GetCurrentTimeString());
    response->set_content("OK.");
    return grpc::Status::OK;
}

grpc::Status ChatServiceImpl::talk(grpc::ServerContext* context, grpc::ServerReaderWriter<ServerWords, ClientWords>* stream)
{
    ClientWords request;
    while (stream->Read(&request)) {
        LOG_INFO("request: {} - \"{}\"", request.timestamp(), request.content());

        ServerWords response;
        response.set_timestamp(utils::GetCurrentTimeString());
        response.set_content(request.content());
        stream->Write(response);
    }

    return grpc::Status::OK;
}

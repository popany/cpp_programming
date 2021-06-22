#include "chat_client.h"
#include <grpcpp/grpcpp.h>
#include "chat.grpc.pb.h"
#include "logger.h"
#include "utils.h"
#include <thread>
#include <chrono>

ChatClient::ChatClient(std::shared_ptr<grpc::Channel> channel) :
    stub(ChatService::NewStub(channel))
{}

void ChatClient::greet()
{
    ClientWords request;
    request.set_timestamp(GetCurrentTimeString());
    request.set_content("Nice to meet you!");

    ServerWords response;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    grpc::ClientContext context;

    grpc::Status status = stub->greet(&context, request, &response);

    if (status.ok()) {
        LOG_INFO("response: {} - \"{}\"", response.timestamp(), response.content());
    } else {
        LOG_ERROR("rpc failed, error_code({}), error_message: \"{}\"", status.error_code(), status.error_message());
    }
}

void ChatClient::listen()
{
    ClientWords request;
    request.set_timestamp(GetCurrentTimeString());
    request.set_content("Speaking!");

    grpc::ClientContext context;
    auto reader = stub->listen(&context, request);

    ServerWords response;
    while (reader->Read(&response)) {
        LOG_INFO("response: {} - \"{}\"", response.timestamp(), response.content());
    }

    grpc::Status status = reader->Finish();
    if (!status.ok()) {
        LOG_ERROR("rpc failed, error_code({}), error_message: \"{}\"", status.error_code(), status.error_message());
    }
}

void ChatClient::speak()
{
    grpc::ClientContext context;
    ServerWords response;
    auto writer = stub->speak(&context, &response);

    ClientWords request;
    request.set_timestamp(GetCurrentTimeString());
    request.set_content("111");
    writer->Write(request);

    request.set_timestamp(GetCurrentTimeString());
    request.set_content("222");
    writer->Write(request);

    request.set_timestamp(GetCurrentTimeString());
    request.set_content("333");
    writer->Write(request);

    writer->WritesDone();
    grpc::Status status = writer->Finish();

    if (status.ok()) {
        LOG_INFO("response: {} - \"{}\"", response.timestamp(), response.content());
    } else {
        LOG_ERROR("rpc failed, error_code({}), error_message: \"{}\"", status.error_code(), status.error_message());
    }
}

void ChatClient::talk()
{
    grpc::ClientContext context;
    std::shared_ptr<grpc::ClientReaderWriter<ClientWords, ServerWords>> stream(stub->talk(&context));

    std::thread tw([stream]() {
        for (int i = 0; i < 10; i++) {
            ClientWords request;
            request.set_timestamp(GetCurrentTimeString());
            request.set_content(std::to_string(i));
            stream->Write(request);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        stream->WritesDone();
    });

    ServerWords response;
    while (stream->Read(&response)) {
        LOG_INFO("response: {} - \"{}\"", response.timestamp(), response.content());
    }

    tw.join();
    grpc::Status status = stream->Finish();
    if (!status.ok()) {
        LOG_ERROR("rpc failed, error_code({}), error_message: \"{}\"", status.error_code(), status.error_message());
    }
}

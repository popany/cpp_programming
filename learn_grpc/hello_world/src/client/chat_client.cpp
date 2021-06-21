#include "chat_client.h"
#include <grpcpp/grpcpp.h>
#include "chat.grpc.pb.h"
#include "logger.h"
#include "utils.h"

ChatClient::ChatClient(std::shared_ptr<grpc::Channel> channel) :
    stub(ChatService::NewStub(channel))
{}

void ChatClient::greet() {
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


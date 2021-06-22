#include "async_chat_client.h"
#include <grpcpp/grpcpp.h>
#include "chat.grpc.pb.h"
#include "logger.h"
#include "utils.h"
#include <functional>

class GreetCall : public AsyncCallResponseProcessor
{
public:
    ServerWords response;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    grpc::ClientContext context;

    // Storage for the status of the RPC upon completion.
    grpc::Status status;

    std::unique_ptr<grpc::ClientAsyncResponseReader<ServerWords>> responseReader;

    void process() override
    {
        if (status.ok()) {
            LOG_INFO("response: {} - \"{}\"", response.timestamp(), response.content());
        } else {
            LOG_ERROR("rpc failed, error_code({}), error_message: \"{}\"", status.error_code(), status.error_message());
        }
    }
};

class ListenCall : public AsyncCallResponseProcessor
{
public:
    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    grpc::ClientContext context;

    // Storage for the status of the RPC upon completion.
    grpc::Status status;

    std::unique_ptr<grpc::ClientAsyncReader<ServerWords>> asyncReader;

    bool readStarted;
    ServerWords response;

    ListenCall() :
        readStarted(false)
    {}

    void process() override
    {
        if (status.ok()) {
            if (readStarted) {
                LOG_INFO("response: {} - \"{}\"", response.timestamp(), response.content());
            }
            LOG_INFO("sssssssssssss");
            asyncReader->Read(&response, this);
            readStarted = true;
        } else {
            LOG_ERROR("rpc failed, error_code({}), error_message: \"{}\"", status.error_code(), status.error_message());
        }
    }
};

AsyncChatClient::AsyncChatClient(std::shared_ptr<grpc::Channel> channel) :
    stub(ChatService::NewStub(channel))
{}

void AsyncChatClient::greet()
{
    ClientWords request;
    request.set_timestamp(utils::GetCurrentTimeString());
    request.set_content("Nice to meet you!");

    std::shared_ptr<GreetCall> call = std::make_shared<GreetCall>();

    call->responseReader = ClientProactor::getInstance().prepareAsyncCall(std::bind(&ChatService::Stub::PrepareAsyncgreet, stub.get(), &call->context, request, std::placeholders::_1),
        call);

    // StartCall initiates the RPC call
    call->responseReader->StartCall();

    // Request that, upon completion of the RPC, "reply" be updated with the
    // server's response; "status" with the indication of whether the operation
    // was successful. Tag the request with the memory address of the call
    // object.
    call->responseReader->Finish(&call->response, &call->status, call.get());
}

void AsyncChatClient::listen()
{
    ClientWords request;
    request.set_timestamp(utils::GetCurrentTimeString());
    request.set_content("Speaking!");

    std::shared_ptr<ListenCall> call = std::make_shared<ListenCall>();

    call->asyncReader = ClientProactor::getInstance().prepareAsyncCall(std::bind(&ChatService::Stub::PrepareAsynclisten, stub.get(), &call->context, request, std::placeholders::_1),
        call);

    // StartCall initiates the RPC call
    call->asyncReader->StartCall(call.get());
}

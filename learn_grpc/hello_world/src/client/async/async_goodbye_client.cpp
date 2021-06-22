#include "async_goodbye_client.h"
#include <grpcpp/grpcpp.h>
#include "goodbye.grpc.pb.h"
#include "logger.h"
#include <functional>

class SayGoodbyeCall : public AsyncCallResponseProcessor
{
public:
    GoodbyeResponse response;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    grpc::ClientContext context;

    // Storage for the status of the RPC upon completion.
    grpc::Status status;

    std::unique_ptr<grpc::ClientAsyncResponseReader<GoodbyeResponse>> responseReader;

    void process() override
    {
        if (status.ok()) {
            LOG_INFO("response: \"{}\"", response.greeting());
        } else {
            LOG_ERROR("rpc failed, error_code({}), error_message: \"{}\"", status.error_code(), status.error_message());
        }
    }
};

struct SayGoodbyeAgainCall : public AsyncCallResponseProcessor
{
public:
    GoodbyeResponse response;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    grpc::ClientContext context;

    // Storage for the status of the RPC upon completion.
    grpc::Status status;

    std::unique_ptr<grpc::ClientAsyncResponseReader<GoodbyeResponse>> responseReader;

    void process() override
    {
        if (status.ok()) {
            LOG_INFO("response: \"{}\"", response.greeting());
        } else {
            LOG_ERROR("rpc failed, error_code({}), error_message: \"{}\"", status.error_code(), status.error_message());
        }
    }
};

AsyncGoodbyeClient::AsyncGoodbyeClient(std::shared_ptr<grpc::Channel> channel, int threadPoolSize) :
    AbstractAsyncClient(threadPoolSize),
    stub(GoodbyeService::NewStub(channel))
{}

void AsyncGoodbyeClient::sayGoodbye()
{
    GoodbyeRequest request;
    request.set_firstname("Foo");
    request.set_lastname("Bar");

    std::shared_ptr<SayGoodbyeCall> call = std::make_shared<SayGoodbyeCall>();

    // stub->PrepareAsyncSayGoodbye() creates an RPC object, returning
    // an instance to store in "call" but does not actually start the RPC
    // Because we are using the asynchronous API, we need to hold on to
    // the "call" instance in order to get updates on the ongoing RPC.
    call->responseReader = prepareAsyncCall(std::bind(&GoodbyeService::Stub::PrepareAsyncsayGoodbye, stub.get(), &call->context, request, std::placeholders::_1),
        call);

    // StartCall initiates the RPC call
    call->responseReader->StartCall();

    // Request that, upon completion of the RPC, "reply" be updated with the
    // server's response; "status" with the indication of whether the operation
    // was successful. Tag the request with the memory address of the call
    // object.
    call->responseReader->Finish(&call->response, &call->status, call.get());
}

void AsyncGoodbyeClient::sayGoodbyeAgain()
{
    GoodbyeRequest request;
    request.set_firstname("Foo");
    request.set_lastname("Bar");

    std::shared_ptr<SayGoodbyeAgainCall> call = std::make_shared<SayGoodbyeAgainCall>();

    call->responseReader = prepareAsyncCall(std::bind(&GoodbyeService::Stub::PrepareAsyncsayGoodbyeAgain, stub.get(), &call->context, request, std::placeholders::_1),
        call);

    // StartCall initiates the RPC call
    call->responseReader->StartCall();

    // Request that, upon completion of the RPC, "reply" be updated with the
    // server's response; "status" with the indication of whether the operation
    // was successful. Tag the request with the memory address of the call
    // object.
    call->responseReader->Finish(&call->response, &call->status, call.get());
}

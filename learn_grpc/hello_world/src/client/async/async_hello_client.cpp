#include "async_hello_client.h"
#include <grpcpp/grpcpp.h>
#include "hello.grpc.pb.h"
#include "logger.h"
#include <functional>

class SayHelloCall : public AsyncCallResponseProcessor
{
public:
    HelloResponse response;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    grpc::ClientContext context;

    // Storage for the status of the RPC upon completion.
    grpc::Status status;

    std::unique_ptr<grpc::ClientAsyncResponseReader<HelloResponse>> responseReader;

    void process() override
    {
        if (status.ok()) {
            LOG_INFO("response: \"{}\"", response.greeting());
        } else {
            LOG_ERROR("rpc failed, error_code({}), error_message: \"{}\"", status.error_code(), status.error_message());
        }
    }
};

struct SayHelloAgainCall : public AsyncCallResponseProcessor
{
public:
    HelloResponse response;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    grpc::ClientContext context;

    // Storage for the status of the RPC upon completion.
    grpc::Status status;

    std::unique_ptr<grpc::ClientAsyncResponseReader<HelloResponse>> responseReader;

    void process() override
    {
        if (status.ok()) {
            LOG_INFO("response: \"{}\"", response.greeting());
        } else {
            LOG_ERROR("rpc failed, error_code({}), error_message: \"{}\"", status.error_code(), status.error_message());
        }
    }
};

AsyncHelloClient::AsyncHelloClient(std::shared_ptr<grpc::Channel> channel, int threadPoolSize) :
    AbstractAsyncClient(threadPoolSize),
    stub(HelloService::NewStub(channel))
{}

void AsyncHelloClient::sayHello()
{
    HelloRequest request;
    request.set_firstname("Foo");
    request.set_lastname("Bar");

    std::shared_ptr<SayHelloCall> call = std::make_shared<SayHelloCall>();

    // stub->PrepareAsyncSayHello() creates an RPC object, returning
    // an instance to store in "call" but does not actually start the RPC
    // Because we are using the asynchronous API, we need to hold on to
    // the "call" instance in order to get updates on the ongoing RPC.
    call->responseReader = prepareAsyncCall(std::bind(&HelloService::Stub::PrepareAsyncsayHello, stub.get(), &call->context, request, std::placeholders::_1),
        call);

    // StartCall initiates the RPC call
    call->responseReader->StartCall();

    // Request that, upon completion of the RPC, "reply" be updated with the
    // server's response; "status" with the indication of whether the operation
    // was successful. Tag the request with the memory address of the call
    // object.
    call->responseReader->Finish(&call->response, &call->status, call.get());
}

void AsyncHelloClient::sayHelloAgain()
{
    HelloRequest request;
    request.set_firstname("Foo");
    request.set_lastname("Bar");

    std::shared_ptr<SayHelloAgainCall> call = std::make_shared<SayHelloAgainCall>();

    // stub->PrepareAsyncSayHello() creates an RPC object, returning
    // an instance to store in "call" but does not actually start the RPC
    // Because we are using the asynchronous API, we need to hold on to
    // the "call" instance in order to get updates on the ongoing RPC.
    call->responseReader = prepareAsyncCall(std::bind(&HelloService::Stub::PrepareAsyncsayHelloAgain, stub.get(), &call->context, request, std::placeholders::_1),
        call);

    // StartCall initiates the RPC call
    call->responseReader->StartCall();

    // Request that, upon completion of the RPC, "reply" be updated with the
    // server's response; "status" with the indication of whether the operation
    // was successful. Tag the request with the memory address of the call
    // object.
    call->responseReader->Finish(&call->response, &call->status, call.get());
}

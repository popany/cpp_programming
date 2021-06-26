#include "async_hello_client.h"
#include <grpcpp/grpcpp.h>
#include "hello.grpc.pb.h"
#include "logger.h"
#include <functional>
#include "client_event_opt.h"

class SayHelloCall : public EventHandler
{
public:
    HelloResponse response;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    grpc::ClientContext context;

    // Storage for the status of the RPC upon completion.
    grpc::Status status;

    std::unique_ptr<grpc::ClientAsyncResponseReader<HelloResponse>> responseReader;

    void process(bool optOk, Event event) override
    {
        if (optOk) {
            if (status.ok()) {
                LOG_INFO("response: \"{}\"", response.greeting());
            } else {
                LOG_ERROR("rpc failed, error_code({}), error_message: \"{}\"", status.error_code(), status.error_message());
            }
        }
        else {
            LOG_ERROR("operation not ok");
        }
    }

    bool isComplete() override
    {
        return true;
    }
};

struct SayHelloAgainCall : public EventHandler
{
public:
    HelloResponse response;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    grpc::ClientContext context;

    // Storage for the status of the RPC upon completion.
    grpc::Status status;

    std::unique_ptr<grpc::ClientAsyncResponseReader<HelloResponse>> responseReader;

    void process(bool optOk, Event event) override
    {
        if (optOk) {
            if (status.ok()) {
                LOG_INFO("response: \"{}\"", response.greeting());
            } else {
                LOG_ERROR("rpc failed, error_code({}), error_message: \"{}\"", status.error_code(), status.error_message());
            }
        }
        else {
            LOG_ERROR("operation not ok");
        }
    }

    bool isComplete() override
    {
        return true;
    }
};

AsyncHelloClient::AsyncHelloClient(std::shared_ptr<grpc::Channel> channel) :
    stub(HelloService::NewStub(channel))
{}

void AsyncHelloClient::sayHello()
{
    HelloRequest request;
    request.set_firstname("Foo");
    request.set_lastname("Bar");

    std::shared_ptr<SayHelloCall> call = std::make_shared<SayHelloCall>();

    event_key_t key;
    // stub->PrepareAsyncSayHello() creates an RPC object, returning
    // an instance to store in "call" but does not actually start the RPC
    // Because we are using the asynchronous API, we need to hold on to
    // the "call" instance in order to get updates on the ongoing RPC.
    call->responseReader = ClientProactor::getInstance().prepareAsyncCall(std::bind(&HelloService::Stub::PrepareAsyncsayHello, stub.get(), &call->context, request, std::placeholders::_1),
        call, key);

    // StartCall initiates the RPC call
    call->responseReader->StartCall();

    Event event(key);
    event.setOpt(CLIENT_EVENT_OPT::FINISH);
    // Request that, upon completion of the RPC, "reply" be updated with the
    // server's response; "status" with the indication of whether the operation
    // was successful. Tag the request with the memory address of the call
    // object.
    call->responseReader->Finish(&call->response, &call->status, event.getToken());
}

void AsyncHelloClient::sayHelloAgain()
{
    HelloRequest request;
    request.set_firstname("Foo");
    request.set_lastname("Bar");

    std::shared_ptr<SayHelloAgainCall> call = std::make_shared<SayHelloAgainCall>();

    event_key_t key;
    call->responseReader = ClientProactor::getInstance().prepareAsyncCall(std::bind(&HelloService::Stub::PrepareAsyncsayHelloAgain, stub.get(), &call->context, request, std::placeholders::_1),
        call, key);

    // StartCall initiates the RPC call
    call->responseReader->StartCall();

    Event event(key);
    event.setOpt(CLIENT_EVENT_OPT::FINISH);
    // Request that, upon completion of the RPC, "reply" be updated with the
    // server's response; "status" with the indication of whether the operation
    // was successful. Tag the request with the memory address of the call
    // object.
    call->responseReader->Finish(&call->response, &call->status, event.getToken());
}

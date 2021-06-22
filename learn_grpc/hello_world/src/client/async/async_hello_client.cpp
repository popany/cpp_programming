#include "async_hello_client.h"
#include <grpcpp/grpcpp.h>
#include "hello.grpc.pb.h"
#include "logger.h"
#include <functional>

struct SayHelloCall
{
    HelloResponse response;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    grpc::ClientContext context;

    // Storage for the status of the RPC upon completion.
    grpc::Status status;

    std::unique_ptr<grpc::ClientAsyncResponseReader<HelloResponse>> responseReader;
};

struct SayHelloAgainCall
{
    HelloResponse response;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    grpc::ClientContext context;

    // Storage for the status of the RPC upon completion.
    grpc::Status status;

    std::unique_ptr<grpc::ClientAsyncResponseReader<HelloResponse>> responseReader;
};

AsyncHelloClient::AsyncHelloClient(std::shared_ptr<grpc::Channel> channel, int threadPoolSize) :
    AbstractAsyncClient(threadPoolSize),
    stub(HelloService::NewStub(channel))
{}

void AsyncHelloClient::sayHello()
{
    HelloRequest request;
    request.set_firstname("Foo");
    request.set_firstname("Bar");

    SayHelloCall* call = new SayHelloCall();

    // stub->PrepareAsyncSayHello() creates an RPC object, returning
    // an instance to store in "call" but does not actually start the RPC
    // Because we are using the asynchronous API, we need to hold on to
    // the "call" instance in order to get updates on the ongoing RPC.
    call->responseReader = prepareAsyncCall(std::bind(&HelloService::Stub::PrepareAsyncsayHello, stub.get(), &call->context, request, std::placeholders::_1),
        static_cast<void*>(call));

    // StartCall initiates the RPC call
    call->responseReader->StartCall();

    // Request that, upon completion of the RPC, "reply" be updated with the
    // server's response; "status" with the indication of whether the operation
    // was successful. Tag the request with the memory address of the call
    // object.
    call->responseReader->Finish(&call->response, &call->status, static_cast<void*>(call));
}

void AsyncHelloClient::sayHelloAgain()
{
    HelloRequest request;
    request.set_firstname("Foo");
    request.set_firstname("Bar");

    SayHelloAgainCall* call = new SayHelloAgainCall();

    // stub->PrepareAsyncSayHello() creates an RPC object, returning
    // an instance to store in "call" but does not actually start the RPC
    // Because we are using the asynchronous API, we need to hold on to
    // the "call" instance in order to get updates on the ongoing RPC.
    call->responseReader = prepareAsyncCall(std::bind(&HelloService::Stub::PrepareAsyncsayHelloAgain, stub.get(), &call->context, request, std::placeholders::_1),
        static_cast<void*>(call));

    // StartCall initiates the RPC call
    call->responseReader->StartCall();

    // Request that, upon completion of the RPC, "reply" be updated with the
    // server's response; "status" with the indication of whether the operation
    // was successful. Tag the request with the memory address of the call
    // object.
    call->responseReader->Finish(&call->response, &call->status, static_cast<void*>(call));
}


void AsyncHelloClient::processSayHelloResponse(void* token)
{
    ;
}

void AsyncHelloClient::processSayHelloAgainResponse(void* token)
{
    ;
}

#if 0
void AsyncHelloClient::sayHelloAgain()
{
    HelloRequest request;
    request.set_firstname("Foo");
    request.set_firstname("Bar");

    AsyncHelloClientCall* call = new AsyncHelloClientCall();

    // stub_->PrepareAsyncSayHello() creates an RPC object, returning
    // an instance to store in "call" but does not actually start the RPC
    // Because we are using the asynchronous API, we need to hold on to
    // the "call" instance in order to get updates on the ongoing RPC.
    call->responseReader = stub->PrepareAsyncsayHelloAgain(&call->context, request, &cq);

    // StartCall initiates the RPC call
    call->responseReader->StartCall();

    // Request that, upon completion of the RPC, "reply" be updated with the
    // server's response; "status" with the indication of whether the operation
    // was successful. Tag the request with the memory address of the call
    // object.
    call->responseReader->Finish(&call->response, &call->status, static_cast<void*>(call));

    AbstractAsyncClient::addToken(static_cast<void*>(call));
}

void AsyncHelloClient::asyncCompleteRpc()
{
    void* got_tag;
    bool ok = false;

    // Block until the next result is available in the completion queue "cq".
    while (cq_.Next(&got_tag, &ok)) {
        // The tag in this example is the memory location of the call object
        AsyncClientCall* call = static_cast<AsyncClientCall*>(got_tag);

        // Verify that the request was completed successfully. Note that "ok"
        // corresponds solely to the request for updates introduced by Finish().
        GPR_ASSERT(ok);

        if (call->status.ok())
            std::cout << "Greeter received: " << call->reply.message() << std::endl;
        else
            std::cout << "RPC failed" << std::endl;

        // Once we're complete, deallocate the call object.
        delete call;
    }
}

#endif
#include "hello_client.h"
#include <grpcpp/grpcpp.h>
#include "hello.grpc.pb.h"
#include "logger.h"

HelloClient::HelloClient(std::shared_ptr<grpc::Channel> channel) :
    stub(HelloService::NewStub(channel))
{}

void HelloClient::sayHello() {
    HelloRequest request;
    request.set_firstname("Foo");
    request.set_lastname("Bar");

    HelloResponse response;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    grpc::ClientContext context;

    grpc::Status status = stub->sayHello(&context, request, &response);

    if (status.ok()) {
        LOG_INFO("response: \"{}\"", response.greeting());
    } else {
        LOG_ERROR("rpc failed, error_code({}), error_message: \"{}\"", status.error_code(), status.error_message());
    }
}

void HelloClient::sayHelloAgain() {
    HelloRequest request;
    request.set_firstname("Foo");
    request.set_lastname("Bar");

    HelloResponse response;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    grpc::ClientContext context;

    grpc::Status status = stub->sayHelloAgain(&context, request, &response);

    if (status.ok()) {
        LOG_INFO("response: \"{}\"", response.greeting());
    } else {
        LOG_ERROR("rpc failed, error_code({}), error_message: \"{}\"", status.error_code(), status.error_message());
    }
}

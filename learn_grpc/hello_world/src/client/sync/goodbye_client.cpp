#include "goodbye_client.h"
#include <grpcpp/grpcpp.h>
#include "goodbye.grpc.pb.h"
#include "logger.h"

GoodbyeClient::GoodbyeClient(std::shared_ptr<grpc::Channel> channel) :
    stub(GoodbyeService::NewStub(channel))
{}

void GoodbyeClient::sayGoodbye()
{
    GoodbyeRequest request;
    request.set_firstname("Foo");
    request.set_lastname("Bar");

    GoodbyeResponse response;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    grpc::ClientContext context;

    grpc::Status status = stub->sayGoodbye(&context, request, &response);

    if (status.ok()) {
        LOG_INFO("response: \"{}\"", response.greeting());
    } else {
        LOG_ERROR("rpc failed, error_code({}), error_message: \"{}\"", status.error_code(), status.error_message());
    }
}

void GoodbyeClient::sayGoodbyeAgain()
{
    GoodbyeRequest request;
    request.set_firstname("Foo");
    request.set_lastname("Bar");

    GoodbyeResponse response;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    grpc::ClientContext context;

    grpc::Status status = stub->sayGoodbyeAgain(&context, request, &response);

    if (status.ok()) {
        LOG_INFO("response: \"{}\"", response.greeting());
    } else {
        LOG_ERROR("rpc failed, error_code({}), error_message: \"{}\"", status.error_code(), status.error_message());
    }
}

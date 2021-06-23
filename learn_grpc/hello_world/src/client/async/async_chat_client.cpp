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

    void process(bool operationOk) override
    {
        if (operationOk) {
            if (status.ok()) {
                LOG_INFO("response: {} - \"{}\"", response.timestamp(), response.content());
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

class ListenCall : public AsyncCallResponseProcessor
{
    enum CALL_STATE
    {
        IDLE = 0,
        START_CALL,
        START_READ,
        END_READ,
        END_CALL,
    };

    CALL_STATE callState;

public:
    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    grpc::ClientContext context;

    // Storage for the status of the RPC upon completion.
    grpc::Status status;

    std::unique_ptr<grpc::ClientAsyncReader<ServerWords>> asyncReader;

    ServerWords response;

    ListenCall() :
        callState(CALL_STATE::IDLE)
    {}

    void process(bool operationOk) override
    {
        if (callState == CALL_STATE::START_CALL) {
            if (operationOk) {
                callState = CALL_STATE::START_READ;
                asyncReader->Read(&response, this);
                LOG_DEBUG("start read, token({})", utils::PtrToHex(this));
            }
            else {
                LOG_ERROR("StartCall operation not ok, token({})", utils::PtrToHex(this));
                callState = CALL_STATE::END_CALL;
            }
        }
        else if (callState == CALL_STATE::START_READ) {
            if (operationOk) {
                LOG_INFO("response: {} - \"{}\", token({})", response.timestamp(), response.content(), utils::PtrToHex(this));
                asyncReader->Read(&response, this);
            }
            else {
                LOG_DEBUG("read end, token({})", utils::PtrToHex(this));
                callState = CALL_STATE::END_READ;
                asyncReader->Finish(&status, this);
            }
        }
        else if (callState == CALL_STATE::END_READ) {
            if (operationOk) {
                if (status.ok()) {
                    LOG_INFO("end call, token({})", utils::PtrToHex(this));
                } else {
                    LOG_ERROR("rpc failed, token({}), error_code({}), error_message: \"{}\"", utils::PtrToHex(this), status.error_code(), status.error_message());
                }
            }
            else {
                LOG_ERROR("Finish operation not ok, token({})", utils::PtrToHex(this));
            }
            callState = CALL_STATE::END_CALL;
        }
        else {
            LOG_ERROR("unexpected routine, token({}), callState({})", utils::PtrToHex(this), callState);
        }
    }

    bool isComplete() override
    {
        return callState == CALL_STATE::END_CALL;
    }

    void start()
    {
        callState = CALL_STATE::START_CALL;
        asyncReader->StartCall(this);
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

    call->start();
}

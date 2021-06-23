#include "async_chat_client.h"
#include <grpcpp/grpcpp.h>
#include "chat.grpc.pb.h"
#include "logger.h"
#include "utils.h"
#include <functional>

class GreetCall : public EventHandler
{
public:
    ServerWords response;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    grpc::ClientContext context;

    // Storage for the status of the RPC upon completion.
    grpc::Status status;

    std::unique_ptr<grpc::ClientAsyncResponseReader<ServerWords>> responseReader;

    void process(bool optOk, Event event) override
    {
        if (optOk) {
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

AsyncChatClient::AsyncChatClient(std::shared_ptr<grpc::Channel> channel) :
    stub(ChatService::NewStub(channel))
{}

void AsyncChatClient::greet()
{
    ClientWords request;
    request.set_timestamp(utils::GetCurrentTimeString());
    request.set_content("Nice to meet you!");

    std::shared_ptr<GreetCall> call = std::make_shared<GreetCall>();

    event_key_t key;
    call->responseReader = ClientProactor::getInstance().prepareAsyncCall(std::bind(&ChatService::Stub::PrepareAsyncgreet, stub.get(), &call->context, request, std::placeholders::_1),
        call, key);

    // StartCall initiates the RPC call
    call->responseReader->StartCall();

    Event event(key);
    event.setOpt(EVENT_OPT::FINISH);
    // Request that, upon completion of the RPC, "reply" be updated with the
    // server's response; "status" with the indication of whether the operation
    // was successful. Tag the request with the memory address of the call
    // object.
    call->responseReader->Finish(&call->response, &call->status, event.getToken());
}

class ListenCall : public EventHandler
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

    void process(bool optOk, Event event) override
    {
        if (callState == CALL_STATE::START_CALL) {
            if (optOk) {
                callState = CALL_STATE::START_READ;
                asyncReader->Read(&response, event.getToken());
                LOG_DEBUG("start read, key({})", event.getKey());
            }
            else {
                LOG_ERROR("StartCall operation not ok, key({})", event.getKey());
                callState = CALL_STATE::END_CALL;
            }
        }
        else if (callState == CALL_STATE::START_READ) {
            if (optOk) {
                LOG_INFO("response: {} - \"{}\", key({})", response.timestamp(), response.content(), event.getKey());
                asyncReader->Read(&response, event.getToken());
            }
            else {
                LOG_DEBUG("read end, key({})", event.getKey());
                callState = CALL_STATE::END_READ;
                asyncReader->Finish(&status, event.getToken());
            }
        }
        else if (callState == CALL_STATE::END_READ) {
            if (optOk) {
                if (status.ok()) {
                    LOG_INFO("end call, key({})", event.getKey());
                } else {
                    LOG_ERROR("rpc failed, key({}), error_code({}), error_message: \"{}\"", event.getKey(), status.error_code(), status.error_message());
                }
            }
            else {
                LOG_ERROR("Finish operation not ok, key({})", event.getKey());
            }
            callState = CALL_STATE::END_CALL;
        }
        else {
            LOG_ERROR("unexpected routine, key({}), callState({})", event.getKey(), callState);
        }
    }

    bool isComplete() override
    {
        return callState == CALL_STATE::END_CALL;
    }

    void start(event_key_t key)
    {
        callState = CALL_STATE::START_CALL;
        Event event(key);
        event.setOpt(EVENT_OPT::START_CALL);
        asyncReader->StartCall(event.getToken());
    }
};

void AsyncChatClient::listen()
{
    ClientWords request;
    request.set_timestamp(utils::GetCurrentTimeString());
    request.set_content("Speaking!");

    std::shared_ptr<ListenCall> call = std::make_shared<ListenCall>();

    event_key_t key;
    call->asyncReader = ClientProactor::getInstance().prepareAsyncCall(std::bind(&ChatService::Stub::PrepareAsynclisten, stub.get(), &call->context, request, std::placeholders::_1),
        call, key);

    call->start(key);
}

#if 0
class SpeakCall : public AsyncCallResponseProcessor, public AsyncChatWriter<ClientWords>
{
    enum CALL_STATE
    {
        IDLE = 0,
        START_CALL,
        END_CALL,
    };

    CALL_STATE callState;

public:
    grpc::ClientContext context;

    grpc::Status status;

    std::unique_ptr<grpc::ClientAsyncWriter<ClientWords>> asyncWriter;

    ServerWords response;

    SpeakCall() :
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
        return callState == callState::END_CALL;
    }

    void start()
    {
        callState = CALL_STATE::START_CALL;
        asyncWriter->StartCall(this);
    }

    bool write(const ClientWords& clientWords) override
    {
        if (writeState == WRITE_STATE::END_WRITE) {
            LOG_WARN("write end, token({})", utils::PtrToHex(this));
            return false;
        }
        if (writeState != WRITE_STATE::READ_FOR_WRITE) {
            LOG_WARN("unexpected runtine, token({})", utils::PtrToHex(this));
            return false;
        }
        asyncWriter->Write(clientWords, this);
        return true;
    }

    void close() override
    {
        if (writeState == WRITE_STATE::END_WRITE) {
            LOG_DEBUG("already write end, token({})", utils::PtrToHex(this));
            return;
        }
        else {
            LOG_INFO("write end, token({})", utils::PtrToHex(this));
            writeState = WRITE_STATE::END_WRITE;
            asyncWriter->Finish(&status, this);
        }
    }
};

AsyncChatWriter& AsyncChatClient::speak()
{
    ClientWords request;
    request.set_timestamp(utils::GetCurrentTimeString());
    request.set_content("Speaking!");

    std::shared_ptr<ListenCall> call = std::make_shared<ListenCall>();

    //PrepareAsyncspeak(::grpc::ClientContext* context, ::ServerWords* response, ::grpc::CompletionQueue* cq) {
    call->asyncWriter = ClientProactor::getInstance().prepareAsyncCall(std::bind(&ChatService::Stub::PrepareAsyncspeak, stub.get(), &call->context, &call->response, std::placeholders::_1),
        call);

    call->start();
    return *this;
}
#endif

#include "async_chat_client.h"
#include <grpcpp/grpcpp.h>
#include "chat.grpc.pb.h"
#include "logger.h"
#include "utils.h"
#include <functional>

class GreetCall : public EventHandler
{
public:
    event_key_t key;
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

    call->responseReader = ClientProactor::getInstance().prepareAsyncCall(std::bind(&ChatService::Stub::PrepareAsyncgreet, stub.get(), &call->context, request, std::placeholders::_1),
        call, call->key);

    // StartCall initiates the RPC call
    call->responseReader->StartCall();

    Event event(call->key);
    event.setOpt(EVENT_OPT::FINISH);
    // Request that, upon completion of the RPC, "reply" be updated with the
    // server's response; "status" with the indication of whether the operation
    // was successful. Tag the request with the memory address of the call
    // object.
    call->responseReader->Finish(&call->response, &call->status, event.getToken());
}

class ListenCall : public EventHandler
{
    bool finish;

public:
    event_key_t key;
    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    grpc::ClientContext context;

    // Storage for the status of the RPC upon completion.
    grpc::Status status;

    std::unique_ptr<grpc::ClientAsyncReader<ServerWords>> asyncReader;

    ServerWords response;

    ListenCall() :
        finish(false)
    {}

    void process(bool optOk, Event event) override
    {
        if (key != event.getKey()) {
            throw std::runtime_error("wrong key");
        }

        switch(event.getOpt()) {
            case EVENT_OPT::START_CALL:
            {
                if (optOk) {
                    event.setOpt(EVENT_OPT::READ);
                    asyncReader->Read(&response, event.getToken());
                    LOG_DEBUG("start read, key({})", event.getKey());
                }
                else {
                    LOG_ERROR("StartCall operation not ok, key({})", event.getKey());
                    finish = true;
                }
            }
            break;
            case EVENT_OPT::READ:
            {
                if (optOk) {
                    LOG_INFO("response: {} - \"{}\", key({})", response.timestamp(), response.content(), event.getKey());
                    asyncReader->Read(&response, event.getToken());
                }
                else {
                    LOG_DEBUG("read end, key({})", event.getKey());
                    event.setOpt(EVENT_OPT::FINISH);
                    asyncReader->Finish(&status, event.getToken());
                }
            }
            break;
            case EVENT_OPT::FINISH:
            {
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
                finish = true;
            }
            break;
            default:
                LOG_ERROR("unexpected routine, key({}), opt({})", event.getKey(), event.getOpt());
        }
    }

    bool isComplete() override
    {
        return finish;
    }

    void start()
    {
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

    call->asyncReader = ClientProactor::getInstance().prepareAsyncCall(std::bind(&ChatService::Stub::PrepareAsynclisten, stub.get(), &call->context, request, std::placeholders::_1),
        call, call->key);

    call->start();
}

class SpeakCall : public EventHandler, public AsyncChatWriter<const std::string&>
{
    bool finish;
    bool closed;

public:
    event_key_t key;
    grpc::ClientContext context;

    grpc::Status status;

    std::unique_ptr<grpc::ClientAsyncWriter<ClientWords>> asyncWriter;

    ServerWords response;

    SpeakCall() :
        finish(false),
        closed(false)
    {}

    void process(bool optOk, Event event) override
    {
                if (key != event.getKey()) {
            throw std::runtime_error("wrong key");
        }

        switch(event.getOpt()) {
            case EVENT_OPT::START_CALL:
            {
                if (optOk) {
                    LOG_DEBUG("StartCall ok, key({})", event.getKey());
                }
                else {
                    LOG_ERROR("StartCall operation not ok, key({})", event.getKey());
                }
            }
            break;
            case EVENT_OPT::WRITE:
            {
                if (optOk) {
                    LOG_DEBUG("Write ok, key({})", event.getKey());
                }
                else {
                    LOG_ERROR("Write operation not ok, key({})", event.getKey());
                }
            }
            break;
            case EVENT_OPT::WRITE_DONE:
            {
                if (optOk) {
                    LOG_DEBUG("WritesDone ok, key({})", event.getKey());
                }
                else {
                    LOG_ERROR("WritesDone operation not ok, key({})", event.getKey());
                }
            }
            break;
            case EVENT_OPT::FINISH:
            {
                if (optOk) {
                    if (status.ok()) {
                        LOG_INFO("Finish, key({}), response: {} - \"{}\"", event.getKey(), response.timestamp(), response.content());
                    } else {
                        LOG_ERROR("rpc failed, key({}), error_code({}), error_message: \"{}\"", event.getKey(), status.error_code(), status.error_message());
                    }
                }
                else {
                    LOG_ERROR("Finish operation not ok, key({})", event.getKey());
                }
                finish = true;
            }
            break;
            default:
                LOG_ERROR("unexpected routine, key({}), opt({})", event.getKey(), event.getOpt());
        }
    }

    bool isComplete() override
    {
        return finish && closed;
    }

    void start()
    {
        Event event(key);
        event.setOpt(EVENT_OPT::START_CALL);
        asyncWriter->StartCall(event.getToken());

        event.setOpt(EVENT_OPT::FINISH);
        asyncWriter->Finish(&status, event.getToken());
    }

    void write(const std::string& msg) override
    {
        Event event(key);
        event.setOpt(EVENT_OPT::WRITE);

        ClientWords request;
        request.set_timestamp(utils::GetCurrentTimeString());
        request.set_content(msg);
        asyncWriter->Write(request, event.getToken());
    }

    void close() override
    {
        Event event(key);
        event.setOpt(EVENT_OPT::WRITE_DONE);
        asyncWriter->WritesDone(event.getToken());
        closed = true;
    }
};

AsyncChatWriter<const std::string&>& AsyncChatClient::speak()
{
    ClientWords request;
    request.set_timestamp(utils::GetCurrentTimeString());
    request.set_content("Speaking!");

    std::shared_ptr<SpeakCall> call = std::make_shared<SpeakCall>();

    call->asyncWriter = ClientProactor::getInstance().prepareAsyncCall(std::bind(&ChatService::Stub::PrepareAsyncspeak, stub.get(), &call->context, &call->response, std::placeholders::_1),
        call, call->key);

    call->start();
    return *call;
}

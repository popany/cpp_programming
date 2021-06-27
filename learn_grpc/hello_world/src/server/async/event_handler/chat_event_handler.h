#pragma once

#include <grpcpp/grpcpp.h>
#include "event_handler.h"
#include "event_handler_manager.h"
#include "chat.grpc.pb.h"
#include "server_event_opt.h"
#include "utils.h"
#include <queue>
#include <memory>

class ChatGreetEventHandler : public EventHandler
{
    ChatService::AsyncService *chatService;
    grpc::ServerCompletionQueue *cq;
    grpc::ServerAsyncResponseWriter<ServerWords> asyncWriter;
    grpc::ServerContext context;

    ClientWords request;

    bool finish;

    void registerHandler()
    {
        ServerProactor::getInstance().registerHandler(std::bind(&ChatService::AsyncService::Requestgreet, chatService, &context, &request, &asyncWriter, cq, cq, std::placeholders::_1), std::shared_ptr<EventHandler>(this));
    }
    
public:
    ChatGreetEventHandler(ChatService::AsyncService *chatService, grpc::ServerCompletionQueue *cq) :
        chatService(chatService),
        cq(cq),
        asyncWriter(&context),
        finish(false)
    {
        registerHandler();
    }

    void process(bool optOk, Event event) override 
    {
        switch(event.getOpt()) {
            case SERVER_EVENT_OPT::RECEIVE:
            {
                new ChatGreetEventHandler(chatService, cq);
                if (optOk) {
                    LOG_INFO("Greet request: {} - \"{}\"", request.timestamp(), request.content());
                    ServerWords response;
                    response.set_timestamp(utils::GetCurrentTimeString());
                    response.set_content("Me too!");
                    event.setOpt(SERVER_EVENT_OPT::FINISH);
                    asyncWriter.Finish(response, grpc::Status::OK, event.getToken());
                }
                else {
                    LOG_ERROR("operation not ok");
                    finish = true;
                }
            }
            break;
            case SERVER_EVENT_OPT::FINISH:
            {
                if (!optOk) {
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

};

class ChatListenEventHandler : public EventHandler
{
    ChatService::AsyncService *chatService;
    grpc::ServerCompletionQueue *cq;
    grpc::ServerAsyncWriter<ServerWords> asyncWriter;
    grpc::ServerContext context;

    ClientWords request;

    std::queue<ServerWords> responses;

    bool finish;

    void registerHandler()
    {
        ServerProactor::getInstance().registerHandler(std::bind(&ChatService::AsyncService::Requestlisten, chatService, &context, &request, &asyncWriter, cq, cq, std::placeholders::_1), std::shared_ptr<EventHandler>(this));
    }
    
public:
    ChatListenEventHandler(ChatService::AsyncService *chatService, grpc::ServerCompletionQueue *cq) :
        chatService(chatService),
        cq(cq),
        asyncWriter(&context),
        finish(false)
    {
        registerHandler();
    }

    void process(bool optOk, Event event) override 
    {
        switch(event.getOpt()) {
            case SERVER_EVENT_OPT::RECEIVE:
            {
                new ChatListenEventHandler(chatService, cq);
                if (optOk) {
                    LOG_INFO("Listen request: {} - \"{}\"", request.timestamp(), request.content());
                    ServerWords response;
                    response.set_timestamp(utils::GetCurrentTimeString());
                    response.set_content("aaa");
                    responses.push(response);

                    response.set_timestamp(utils::GetCurrentTimeString());
                    response.set_content("bbb");
                    responses.push(response);

                    response.set_timestamp(utils::GetCurrentTimeString());
                    response.set_content("ccc");
                    responses.push(response);

                    event.setOpt(SERVER_EVENT_OPT::WRITE);
                    asyncWriter.Write(responses.front(), event.getToken());
                    responses.pop();
                }
                else {
                    LOG_ERROR("Receive operation not ok");
                    finish = true;
                }
            }
            break;
            case SERVER_EVENT_OPT::WRITE:
            {
                if (optOk) {
                    if (responses.empty()) {
                        event.setOpt(SERVER_EVENT_OPT::FINISH);
                        asyncWriter.Finish(grpc::Status::OK, event.getToken());
                    }
                    else {
                        event.setOpt(SERVER_EVENT_OPT::WRITE);
                        asyncWriter.Write(responses.front(), event.getToken());
                        responses.pop();
                    }
                }
                else {
                    LOG_ERROR("Write operation not ok");
                    finish = true;
                }
            }
            break;
            case SERVER_EVENT_OPT::FINISH:
            {
                if (!optOk) {
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

};

class ChatSpeakEventHandler : public EventHandler
{
    ChatService::AsyncService *chatService;
    grpc::ServerCompletionQueue *cq;
    grpc::ServerAsyncReader<ServerWords, ClientWords> asyncReader;
    grpc::ServerContext context;

    ClientWords request;

    bool finish;

    void registerHandler()
    {
        ServerProactor::getInstance().registerHandler(std::bind(&ChatService::AsyncService::Requestspeak, chatService, &context, &asyncReader, cq, cq, std::placeholders::_1), std::shared_ptr<EventHandler>(this));
    }
    
public:
    ChatSpeakEventHandler(ChatService::AsyncService *chatService, grpc::ServerCompletionQueue *cq) :
        chatService(chatService),
        cq(cq),
        asyncReader(&context),
        finish(false)
    {
        registerHandler();
    }

    void process(bool optOk, Event event) override 
    {
        switch(event.getOpt()) {
            case SERVER_EVENT_OPT::RECEIVE:
            {
                new ChatSpeakEventHandler(chatService, cq);
                if (optOk) {
                    event.setOpt(SERVER_EVENT_OPT::READ);
                    asyncReader.Read(&request, event.getToken());
                }
                else {
                    LOG_ERROR("Receive operation not ok");
                    finish = true;
                }
            }
            break;
            case SERVER_EVENT_OPT::READ:
            {
                if (optOk) {
                    LOG_INFO("Speak request: {} - \"{}\"", request.timestamp(), request.content());
                    asyncReader.Read(&request, event.getToken());
                }
                else {
                    ServerWords response;
                    response.set_timestamp(utils::GetCurrentTimeString());
                    response.set_content("OK.");
                    event.setOpt(SERVER_EVENT_OPT::FINISH);
                    asyncReader.Finish(response, grpc::Status::OK, event.getToken());
                }
            }
            break;
            case SERVER_EVENT_OPT::FINISH:
            {
                if (!optOk) {
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

};

class ChatTalkEventHandler : public EventHandler
{
    ChatService::AsyncService *chatService;
    grpc::ServerCompletionQueue *cq;
    grpc::ServerAsyncReaderWriter<ServerWords, ClientWords> asyncReaderWriter;
    grpc::ServerContext context;

    ClientWords request;

    bool finish;

    void registerHandler()
    {
        ServerProactor::getInstance().registerHandler(std::bind(&ChatService::AsyncService::Requesttalk, chatService, &context, &asyncReaderWriter, cq, cq, std::placeholders::_1), std::shared_ptr<EventHandler>(this));
    }
    
public:
    ChatTalkEventHandler(ChatService::AsyncService *chatService, grpc::ServerCompletionQueue *cq) :
        chatService(chatService),
        cq(cq),
        asyncReaderWriter(&context),
        finish(false)
    {
        registerHandler();
    }

    void process(bool optOk, Event event) override 
    {
        switch(event.getOpt()) {
            case SERVER_EVENT_OPT::RECEIVE:
            {
                new ChatTalkEventHandler(chatService, cq);
                if (optOk) {
                    LOG_DEBUG("Talk recieve, key({})", event.getKey());
                    event.setOpt(SERVER_EVENT_OPT::READ);
                    asyncReaderWriter.Read(&request, event.getToken());
                }
                else {
                    LOG_ERROR("Talk receive operation not ok, key({})", event.getKey());
                    finish = true;
                }
            }
            break;
            case SERVER_EVENT_OPT::READ:
            {
                if (optOk) {
                    LOG_INFO("Talk request: {} - \"{}\", key({})", request.timestamp(), request.content(), event.getKey());

                    ServerWords response;
                    response.set_timestamp(utils::GetCurrentTimeString());
                    response.set_content(request.content());
                    event.setOpt(SERVER_EVENT_OPT::WRITE);
                    asyncReaderWriter.Write(response, event.getToken());
                }
                else {
                    LOG_INFO("Talk read end, key({})", event.getKey());
                    event.setOpt(SERVER_EVENT_OPT::FINISH);
                    asyncReaderWriter.Finish(grpc::Status::OK, event.getToken());
                }
            }
            break;
            case SERVER_EVENT_OPT::WRITE:
            {
                if (optOk) {
                    LOG_DEBUG("Talk write, key({})", event.getKey());
                    event.setOpt(SERVER_EVENT_OPT::READ);
                    asyncReaderWriter.Read(&request, event.getToken());
                }
                else {
                    LOG_ERROR("Talk write operation not ok, key({})", event.getKey());
                    asyncReaderWriter.Finish(grpc::Status::CANCELLED, event.getToken());
                }
            }
            break;
            case SERVER_EVENT_OPT::FINISH:
            {
                LOG_DEBUG("Talk finish, key({})", event.getKey());
                if (!optOk) {
                    LOG_ERROR("Finish operation not ok, key({})", event.getKey());
                }
                finish = true;
            }
            break;
            default:
                LOG_ERROR("unexpected routine, key({}), opt({})", event.getKey(), event.getOpt());
                finish = true;
        }
    }

    bool isComplete() override
    {
        return finish;
    }

};

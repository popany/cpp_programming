#pragma once

#include <grpcpp/grpcpp.h>
#include "event_handler.h"
#include "event_handler_manager.h"
#include "chat.grpc.pb.h"
#include "server_event_opt.h"
#include "utils.h"
#include <queue>

class ChatGreetEventHandler : public EventHandler
{
    ChatService::AsyncService *chatService;
    grpc::ServerCompletionQueue *cq;
    EventHandlerManager *handlerManager;
    grpc::ServerAsyncResponseWriter<ServerWords> asyncWriter;
    grpc::ServerContext context;

    ClientWords request;

    bool finish;
    
public:
    ChatGreetEventHandler(ChatService::AsyncService *chatService, grpc::ServerCompletionQueue *cq, EventHandlerManager *handlerManager) :
        chatService(chatService),
        cq(cq),
        handlerManager(handlerManager),
        asyncWriter(&context),
        finish(false)
    {
        Event event = handlerManager->add(std::shared_ptr<ChatGreetEventHandler>(this));
        event.setOpt(SERVER_EVENT_OPT::RECEIVE);
        chatService->Requestgreet(&context, &request, &asyncWriter, cq, cq, event.getToken());
    }

    void process(bool optOk, Event event) override 
    {
        switch(event.getOpt()) {
            case SERVER_EVENT_OPT::RECEIVE:
            {
                new ChatGreetEventHandler(chatService, cq, handlerManager);
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
    EventHandlerManager *handlerManager;
    grpc::ServerAsyncWriter<ServerWords> asyncWriter;
    grpc::ServerContext context;

    ClientWords request;

    std::queue<ServerWords> responses;

    bool finish;
    
public:
    ChatListenEventHandler(ChatService::AsyncService *chatService, grpc::ServerCompletionQueue *cq, EventHandlerManager *handlerManager) :
        chatService(chatService),
        cq(cq),
        handlerManager(handlerManager),
        asyncWriter(&context),
        finish(false)
    {
        Event event = handlerManager->add(std::shared_ptr<ChatListenEventHandler>(this));
        event.setOpt(SERVER_EVENT_OPT::RECEIVE);
        chatService->Requestlisten(&context, &request, &asyncWriter, cq, cq, event.getToken());
    }

    void process(bool optOk, Event event) override 
    {
        switch(event.getOpt()) {
            case SERVER_EVENT_OPT::RECEIVE:
            {
                new ChatListenEventHandler(chatService, cq, handlerManager);
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
    EventHandlerManager *handlerManager;
    grpc::ServerAsyncReader<ServerWords, ClientWords> asyncReader;
    grpc::ServerContext context;

    ClientWords request;

    bool finish;
    
public:
    ChatSpeakEventHandler(ChatService::AsyncService *chatService, grpc::ServerCompletionQueue *cq, EventHandlerManager *handlerManager) :
        chatService(chatService),
        cq(cq),
        handlerManager(handlerManager),
        asyncReader(&context),
        finish(false)
    {
        Event event = handlerManager->add(std::shared_ptr<ChatSpeakEventHandler>(this));
        event.setOpt(SERVER_EVENT_OPT::RECEIVE);
        chatService->Requestspeak(&context, &asyncReader, cq, cq, event.getToken());
    }

    void process(bool optOk, Event event) override 
    {
        switch(event.getOpt()) {
            case SERVER_EVENT_OPT::RECEIVE:
            {
                new ChatSpeakEventHandler(chatService, cq, handlerManager);
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
    EventHandlerManager *handlerManager;
    grpc::ServerAsyncReaderWriter<ServerWords, ClientWords> asyncReaderWriter;
    grpc::ServerContext context;

    ClientWords request;

    bool finish;
    
public:
    ChatTalkEventHandler(ChatService::AsyncService *chatService, grpc::ServerCompletionQueue *cq, EventHandlerManager *handlerManager) :
        chatService(chatService),
        cq(cq),
        handlerManager(handlerManager),
        asyncReaderWriter(&context),
        finish(false)
    {
        Event event = handlerManager->add(std::shared_ptr<ChatTalkEventHandler>(this));
        event.setOpt(SERVER_EVENT_OPT::RECEIVE);
        chatService->Requesttalk(&context, &asyncReaderWriter, cq, cq, event.getToken());
    }

    void process(bool optOk, Event event) override 
    {
        switch(event.getOpt()) {
            case SERVER_EVENT_OPT::RECEIVE:
            {
                new ChatTalkEventHandler(chatService, cq, handlerManager);
                if (optOk) {
                    LOG_DEBUG("Talk recieve, key({})", event.getKey());
                    event.setOpt(SERVER_EVENT_OPT::READ);
                    asyncReaderWriter.Read(&request, event.getToken());
                }
                else {
                    LOG_ERROR("Talk receive operation not ok, key({})", event.getKey());
                    asyncReaderWriter.Finish(grpc::Status::CANCELLED, event.getToken());
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

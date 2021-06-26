#pragma once

#include <grpcpp/grpcpp.h>
#include "event_handler.h"
#include "event_handler_manager.h"
#include "chat.grpc.pb.h"
#include "server_event_opt.h"
#include "utils.h"

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
                    LOG_INFO("request: {} - \"{}\"", request.timestamp(), request.content());
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


#pragma once

#include <grpcpp/grpcpp.h>
#include "event_handler.h"
#include "event_handler_manager.h"
#include "goodbye.grpc.pb.h"
#include "server_event_opt.h"

class GoodbyeEventHandler : public EventHandler
{
    GoodbyeService::AsyncService *goodbyeService;
    grpc::ServerCompletionQueue *cq;
    EventHandlerManager *handlerManager;

    grpc::ServerContext context;
    grpc::ServerAsyncResponseWriter<GoodbyeResponse> asyncWriter;
    GoodbyeRequest request;

    bool finish;
    
public:
    GoodbyeEventHandler(GoodbyeService::AsyncService *goodbyeService, grpc::ServerCompletionQueue *cq, EventHandlerManager *handlerManager) :
        goodbyeService(goodbyeService),
        cq(cq),
        handlerManager(handlerManager),
        asyncWriter(&context),
        finish(false)
    {
        Event event = handlerManager->add(std::shared_ptr<GoodbyeEventHandler>(this));
        event.setOpt(SERVER_EVENT_OPT::RECEIVE);
        goodbyeService->RequestsayGoodbye(&context, &request, &asyncWriter, cq, cq, event.getToken());
    }

    void process(bool optOk, Event event) override 
    {
        switch(event.getOpt()) {
            case SERVER_EVENT_OPT::RECEIVE:
            {
                new GoodbyeEventHandler(goodbyeService, cq, handlerManager);
                if (optOk) {
                    LOG_INFO("SayGoodbye request, firstname: {}, lastname: {}", request.firstname(), request.lastname());
                    GoodbyeResponse response;
                    response.set_greeting(std::string("Goodbye, ") + request.firstname() + " " + request.lastname());
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

class GoodbyeAgainEventHandler : public EventHandler
{
    GoodbyeService::AsyncService *goodbyeService;
    grpc::ServerCompletionQueue *cq;
    EventHandlerManager *handlerManager;

    grpc::ServerContext context;
    grpc::ServerAsyncResponseWriter<GoodbyeResponse> asyncWriter;
    GoodbyeRequest request;
    
    bool finish;

public:
    GoodbyeAgainEventHandler(GoodbyeService::AsyncService *goodbyeService, grpc::ServerCompletionQueue *cq, EventHandlerManager *handlerManager) :
        goodbyeService(goodbyeService),
        cq(cq),
        handlerManager(handlerManager),
        asyncWriter(&context),
        finish(false)
    {
        Event event = handlerManager->add(std::shared_ptr<GoodbyeAgainEventHandler>(this));
        event.setOpt(SERVER_EVENT_OPT::RECEIVE);
        goodbyeService->RequestsayGoodbyeAgain(&context, &request, &asyncWriter, cq, cq, event.getToken());
    }

    void process(bool optOk, Event event) override 
    {
        switch(event.getOpt()) {
            case SERVER_EVENT_OPT::RECEIVE:
            {
                new GoodbyeAgainEventHandler(goodbyeService, cq, handlerManager);
                if (optOk) {
                    LOG_INFO("SayGoodbyeAgain request, firstname: {}, lastname: {}", request.firstname(), request.lastname());
                    GoodbyeResponse response;
                    response.set_greeting(std::string("Goodbye, ") + request.firstname() + " " + request.lastname());
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

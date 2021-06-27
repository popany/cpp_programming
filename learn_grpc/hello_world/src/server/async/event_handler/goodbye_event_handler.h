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

    grpc::ServerContext context;
    grpc::ServerAsyncResponseWriter<GoodbyeResponse> asyncWriter;
    GoodbyeRequest request;

    bool finish;

    void registerHandler()
    {
        ServerProactor::getInstance().registerHandler(std::bind(&GoodbyeService::AsyncService::RequestsayGoodbye, goodbyeService, &context, &request, &asyncWriter, cq, cq, std::placeholders::_1), std::shared_ptr<EventHandler>(this));
    }
    
public:
    GoodbyeEventHandler(GoodbyeService::AsyncService *goodbyeService, grpc::ServerCompletionQueue *cq) :
        goodbyeService(goodbyeService),
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
                new GoodbyeEventHandler(goodbyeService, cq);
                if (optOk) {
                    LOG_INFO("SayGoodbye request, firstname: {}, lastname: {}", request.firstname(), request.lastname());
                    GoodbyeResponse response;
                    response.set_greeting(std::string("Goodbye, ") + request.firstname() + " " + request.lastname());
                    event.setOpt(SERVER_EVENT_OPT::FINISH);
                    asyncWriter.Finish(response, grpc::Status::OK, event.getToken());
                }
                else {
                    LOG_ERROR("Receive operation not ok");
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

    grpc::ServerContext context;
    grpc::ServerAsyncResponseWriter<GoodbyeResponse> asyncWriter;
    GoodbyeRequest request;
    
    bool finish;

    void registerHandler()
    {
        ServerProactor::getInstance().registerHandler(std::bind(&GoodbyeService::AsyncService::RequestsayGoodbyeAgain, goodbyeService, &context, &request, &asyncWriter, cq, cq, std::placeholders::_1), std::shared_ptr<EventHandler>(this));
    }

public:
    GoodbyeAgainEventHandler(GoodbyeService::AsyncService *goodbyeService, grpc::ServerCompletionQueue *cq) :
        goodbyeService(goodbyeService),
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
                new GoodbyeAgainEventHandler(goodbyeService, cq);
                if (optOk) {
                    LOG_INFO("SayGoodbyeAgain request, firstname: {}, lastname: {}", request.firstname(), request.lastname());
                    GoodbyeResponse response;
                    response.set_greeting(std::string("Goodbye, ") + request.firstname() + " " + request.lastname());
                    event.setOpt(SERVER_EVENT_OPT::FINISH);
                    asyncWriter.Finish(response, grpc::Status::OK, event.getToken());
                }
                else {
                    LOG_ERROR("Receive operation not ok");
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

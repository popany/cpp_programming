#pragma once

#include <grpcpp/grpcpp.h>
#include "event_handler.h"
#include "event_handler_manager.h"
#include "hello.grpc.pb.h"
#include "server_event_opt.h"

class HelloEventHandler : public EventHandler
{
    HelloService::AsyncService *helloService;
    grpc::ServerCompletionQueue *cq;
    EventHandlerManager *handlerManager;

    grpc::ServerContext context;
    grpc::ServerAsyncResponseWriter<HelloResponse> asyncWriter;
    HelloRequest request;

    bool finish;
    
public:
    HelloEventHandler(HelloService::AsyncService *helloService, grpc::ServerCompletionQueue *cq, EventHandlerManager *handlerManager) :
        helloService(helloService),
        cq(cq),
        handlerManager(handlerManager),
        asyncWriter(&context),
        finish(false)
    {
        Event event = handlerManager->add(std::shared_ptr<HelloEventHandler>(this));
        event.setOpt(SERVER_EVENT_OPT::RECEIVE);
        helloService->RequestsayHello(&context, &request, &asyncWriter, cq, cq, event.getToken());
    }

    void process(bool optOk, Event event) override 
    {
        switch(event.getOpt()) {
            case SERVER_EVENT_OPT::RECEIVE:
            {
                new HelloEventHandler(helloService, cq, handlerManager);
                if (optOk) {
                    LOG_INFO("SayHello request, firstname: {}, lastname: {}", request.firstname(), request.lastname());
                    HelloResponse response;
                    response.set_greeting(std::string("Hello, ") + request.firstname() + " " + request.lastname());
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

class HelloAgainEventHandler : public EventHandler
{
    HelloService::AsyncService *helloService;
    grpc::ServerCompletionQueue *cq;
    EventHandlerManager *handlerManager;

    grpc::ServerContext context;
    grpc::ServerAsyncResponseWriter<HelloResponse> asyncWriter;
    HelloRequest request;
    
    bool finish;

public:
    HelloAgainEventHandler(HelloService::AsyncService *helloService, grpc::ServerCompletionQueue *cq, EventHandlerManager *handlerManager) :
        helloService(helloService),
        cq(cq),
        handlerManager(handlerManager),
        asyncWriter(&context),
        finish(false)
    {
        Event event = handlerManager->add(std::shared_ptr<HelloAgainEventHandler>(this));
        event.setOpt(SERVER_EVENT_OPT::RECEIVE);
        helloService->RequestsayHelloAgain(&context, &request, &asyncWriter, cq, cq, event.getToken());
    }

    void process(bool optOk, Event event) override 
    {
        switch(event.getOpt()) {
            case SERVER_EVENT_OPT::RECEIVE:
            {
                new HelloAgainEventHandler(helloService, cq, handlerManager);
                if (optOk) {
                    LOG_INFO("SayHelloAgain request, firstname: {}, lastname: {}", request.firstname(), request.lastname());
                    HelloResponse response;
                    response.set_greeting(std::string("Hello, ") + request.firstname() + " " + request.lastname());
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

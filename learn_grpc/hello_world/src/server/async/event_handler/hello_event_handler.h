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

    grpc::ServerContext context;
    grpc::ServerAsyncResponseWriter<HelloResponse> asyncWriter;
    HelloRequest request;

    bool finish;

    void registerHandler()
    {
        ServerProactor::getInstance().registerHandler(std::bind(&HelloService::AsyncService::RequestsayHello, helloService, &context, &request, &asyncWriter, cq, cq, std::placeholders::_1), std::shared_ptr<EventHandler>(this));
    }
    
public:
    HelloEventHandler(HelloService::AsyncService *helloService, grpc::ServerCompletionQueue *cq) :
        helloService(helloService),
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
                new HelloEventHandler(helloService, cq);
                if (optOk) {
                    LOG_INFO("SayHello request, firstname: {}, lastname: {}", request.firstname(), request.lastname());
                    HelloResponse response;
                    response.set_greeting(std::string("Hello, ") + request.firstname() + " " + request.lastname());
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

class HelloAgainEventHandler : public EventHandler
{
    HelloService::AsyncService *helloService;
    grpc::ServerCompletionQueue *cq;

    grpc::ServerContext context;
    grpc::ServerAsyncResponseWriter<HelloResponse> asyncWriter;
    HelloRequest request;
    
    bool finish;

    void registerHandler()
    {
        ServerProactor::getInstance().registerHandler(std::bind(&HelloService::AsyncService::RequestsayHelloAgain, helloService, &context, &request, &asyncWriter, cq, cq, std::placeholders::_1), std::shared_ptr<EventHandler>(this));
    }

public:
    HelloAgainEventHandler(HelloService::AsyncService *helloService, grpc::ServerCompletionQueue *cq) :
        helloService(helloService),
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
                new HelloAgainEventHandler(helloService, cq);
                if (optOk) {
                    LOG_INFO("SayHelloAgain request, firstname: {}, lastname: {}", request.firstname(), request.lastname());
                    HelloResponse response;
                    response.set_greeting(std::string("Hello, ") + request.firstname() + " " + request.lastname());
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

#pragma once

#include <grpcpp/grpcpp.h>
#include <memory>
#include "hello.grpc.pb.h"
#include "goodbye.grpc.pb.h"
#include "chat.grpc.pb.h"
#include <atomic>
#include "event_handler_manager.h"
#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>
#include <vector>
#include "event_handler/server_event_opt.h"
#include "logger.h"

class ServerProactor
{
    boost::asio::thread_pool threadPool;
    int threadPoolSize;
    EventHandlerManager handlerManager;

    std::vector<std::unique_ptr<grpc::ServerCompletionQueue>> cqs;
    std::atomic_int cqIdx;
    HelloService::AsyncService helloService;
    GoodbyeService::AsyncService goodbyeService;
    ChatService::AsyncService chatService;
    std::atomic_bool stopped;

    ServerProactor(int threadPoolSize);
    void demultiplex();

public:
    ServerProactor(const ServerProactor&) = delete;
    void operator=(const ServerProactor&) = delete;

    void startDemuxer();
    void waitForComplete();

    int getThreadPoolSize()
    {
        return threadPoolSize;
    }

    grpc::Service& getHelloService()
    {
        return helloService;
    }
    
    grpc::Service& getGoodbyeService()
    {
        return goodbyeService;
    }
    
    grpc::Service& getChatService()
    {
        return chatService;
    }
    
    void addCompletionQueue(std::unique_ptr<grpc::ServerCompletionQueue>&& cq)
    {
        cqs.emplace_back(std::move(cq));
    }

    template<class F>
    void registerHandler(F requestFunc, std::shared_ptr<EventHandler> handler) 
    {
        if (stopped) {
            LOG_INFO("stopped");
            return;
        }
        Event event = handlerManager.add(handler);
        event.setOpt(SERVER_EVENT_OPT::RECEIVE);
        requestFunc(event.getToken());
    }

    void stop()
    {
        LOG_INFO("stop");
        stopped = true;
    }

    void shutdown();

    static ServerProactor& getInstance();
};

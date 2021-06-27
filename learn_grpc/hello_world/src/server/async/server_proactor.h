#pragma once

#include <grpcpp/grpcpp.h>
#include "proactor.h"
#include <memory>
#include "hello.grpc.pb.h"
#include "goodbye.grpc.pb.h"
#include "chat.grpc.pb.h"
#include <atomic>
#include "event_handler_manager.h"
#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>
#include <vector>

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

    void shutdown();

    static ServerProactor& getInstance();
};

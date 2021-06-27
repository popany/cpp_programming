#include "server_proactor.h"
#include "../config/server_config.h"
#include "event_handler/chat_event_handler.h"
#include "event_handler/goodbye_event_handler.h"
#include "event_handler/hello_event_handler.h"

ServerProactor::ServerProactor(int threadPoolSize) :
    threadPoolSize(threadPoolSize),
    threadPool(threadPoolSize),
    cqIdx(0),
    stopped(false)
{
    cqs.reserve(threadPoolSize);
}

ServerProactor& ServerProactor::getInstance()
{
    static ServerProactor instance(SERVER_CONFIG.GET_GRPC_SERVER_ASYNC_THREADPOOL_SIZE());
    return instance;
}

void ServerProactor::startDemuxer()
{
    for (int i = 0; i < threadPoolSize; i++) {
        boost::asio::post(threadPool, std::bind(&ServerProactor::demultiplex, this));
    }
}

void ServerProactor::waitForComplete()
{
    threadPool.join();
}

void ServerProactor::demultiplex()
{
    grpc::ServerCompletionQueue* cq = cqs[cqIdx++].get();

    new HelloEventHandler(&helloService, cq, &handlerManager);
    new HelloAgainEventHandler(&helloService, cq, &handlerManager);
    new GoodbyeEventHandler(&goodbyeService, cq, &handlerManager);
    new GoodbyeAgainEventHandler(&goodbyeService, cq, &handlerManager);
    new ChatGreetEventHandler(&chatService, cq, &handlerManager);
    new ChatListenEventHandler(&chatService, cq, &handlerManager);
    new ChatSpeakEventHandler(&chatService, cq, &handlerManager);
    
    void* token;
    bool ok = false;
    while (true) {
        grpc::CompletionQueue::NextStatus nextStatus = cq->AsyncNext(&token, &ok,
            std::chrono::system_clock::now() + std::chrono::milliseconds(SERVER_CONFIG.GET_GRPC_SERVER_ASYNC_POLLING_INTERVAL_MILLISECONDS()));

        if (nextStatus == grpc::CompletionQueue::NextStatus::SHUTDOWN) {
            LOG_INFO("cq shutdown, remaining eventHandler count: {}, keys: {}", handlerManager.count(), handlerManager.getKeys());
            return;
        }
        if (nextStatus == grpc::CompletionQueue::NextStatus::TIMEOUT) {
            LOG_DEBUG("handlerManager count: {}, keys: {}", handlerManager.count(), handlerManager.getKeys());
            if (handlerManager.count()) {
                continue;
            }
            else {
                break;
            }
        }
        Event event(token);
        if (!handlerManager.contains(event.getKey())) {
            LOG_WARN("event key({}) not exist", event.getKey());
            continue;
        }

        if (stopped) {
            LOG_INFO("stopped, ignore key{}", event.getKey());
        }
        else { 
            auto handler = handlerManager.get(event.getKey());
            handler->process(ok, event);

            if (handler->isComplete()) {
                handlerManager.remove(event.getKey());
            }
        }
    }

}

void ServerProactor::shutdown()
{
    stopped = true;
    for (auto& cq : cqs) {
        cq->Shutdown();
    }
}

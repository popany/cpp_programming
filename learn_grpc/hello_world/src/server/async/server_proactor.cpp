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

    new HelloEventHandler(&helloService, cq);
    new HelloAgainEventHandler(&helloService, cq);
    new GoodbyeEventHandler(&goodbyeService, cq);
    new GoodbyeAgainEventHandler(&goodbyeService, cq);
    new ChatGreetEventHandler(&chatService, cq);
    new ChatListenEventHandler(&chatService, cq);
    new ChatSpeakEventHandler(&chatService, cq);
    new ChatTalkEventHandler(&chatService, cq);
    
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

        auto handler = handlerManager.get(event.getKey());
        handler->process(ok, event);

        if (handler->isComplete()) {
            handlerManager.remove(event.getKey());
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

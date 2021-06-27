#include "client_proactor.h"
#include <thread>
#include <chrono>
#include "logger.h"
#include "utils.h"
#include "../config/client_config.h"

ClientProactor::ClientProactor(int threadPoolSize):
    cqs(threadPoolSize),
    threadPoolSize(threadPoolSize),
    threadPool(threadPoolSize),
    cqIdx(0)
{}

void ClientProactor::asyncCompleteRpc(grpc::CompletionQueue& cq)
{
    void* token;
    bool ok = false;
    while (true) {
        grpc::CompletionQueue::NextStatus nextStatus = cq.AsyncNext(&token, &ok,
            std::chrono::system_clock::now() + std::chrono::milliseconds(CLIENT_CONFIG.GET_GRPC_CLIENT_ASYNC_POLLING_INTERVAL_MILLISECONDS()));

        if (nextStatus == grpc::CompletionQueue::NextStatus::SHUTDOWN) {
            LOG_INFO("cq shutdown, remaining eventHandler count: {}, keys: {}", handlerManager.count(), handlerManager.getKeys());
            handlerManager.clear();
            return;
        }
        if (nextStatus == grpc::CompletionQueue::NextStatus::TIMEOUT) {
            LOG_DEBUG("Timeout, handlerManager count: {}, keys: {}", handlerManager.count(), handlerManager.getKeys());
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

void ClientProactor::startThreadPool()
{
    for (int i = 0; i < threadPoolSize; i++) {
        boost::asio::post(threadPool, std::bind(&ClientProactor::asyncCompleteRpc, this, std::ref(cqs[i])));
    }
}

void ClientProactor::waitForComplete()
{
    threadPool.join();
}

ClientProactor& ClientProactor::getInstance()
{
    static ClientProactor instance(CLIENT_CONFIG.GET_GRPC_CLIENT_ASYNC_THREADPOOL_SIZE());
    return instance;
}

#include "client_proactor.h"
#include <thread>
#include <chrono>
#include "logger.h"
#include "utils.h"
#include "../config/client_config.h"

ClientProactor::ClientProactor(int threadPoolSize):
    threadPoolSize(threadPoolSize),
    threadPool(threadPoolSize)
{}

void ClientProactor::addToken(void* token, std::shared_ptr<AsyncCallResponseProcessor> processor)
{
    std::lock_guard<std::mutex> lock(tokensLock);
    tokens.insert({ token, processor });
}

bool ClientProactor::isTokenExist(void* token)
{
    std::lock_guard<std::mutex> lock(tokensLock);
    return tokens.count(token);
}

void ClientProactor::removeToken(void* token)
{
    std::lock_guard<std::mutex> lock(tokensLock);
    tokens.erase(token);
}

void ClientProactor::removeAllTokens()
{
    std::lock_guard<std::mutex> lock(tokensLock);
    tokens.clear();
}

size_t ClientProactor::getTokenCount()
{
    std::lock_guard<std::mutex> lock(tokensLock);
    return tokens.size();
}

std::shared_ptr<AsyncCallResponseProcessor> ClientProactor::getProcesser(void* token)
{
    std::lock_guard<std::mutex> lock(tokensLock);
    return tokens[token];
}

void ClientProactor::asyncCompleteRpc()
{
    void* token;
    bool ok = false;
    while (true) {
        grpc::CompletionQueue::NextStatus nextStatus = cq.AsyncNext(&token, &ok,
            std::chrono::system_clock::now() + std::chrono::milliseconds(CLIENT_CONFIG.GET_GRPC_CLIENT_ASYNC_POLLING_INTERVAL_MILLISECONDS()));

        if (nextStatus == grpc::CompletionQueue::NextStatus::SHUTDOWN) {
            LOG_INFO("cq shutdown, remaining tokens count: {}", getTokenCount());
            removeAllTokens();
            return;
        }
        if (nextStatus == grpc::CompletionQueue::NextStatus::TIMEOUT) {
            if (getTokenCount()) {
                continue;
            }
            else {
                break;
            }
        }
        if (!isTokenExist(token)) {
            LOG_WARN("token({}) unrecognized ", utils::IntToHex((size_t)token));
            continue;
        }
        if (!ok) {
            LOG_ERROR("token({}) not ok", utils::IntToHex((size_t)token));
            removeToken(token);
            continue;
        }
        getProcesser(token)->process();
        removeToken(token);
    }
}

void ClientProactor::startThreadPool()
{
    for (int i = 0; i < threadPoolSize; i++) {
        boost::asio::post(threadPool, std::bind(&ClientProactor::asyncCompleteRpc, this));
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

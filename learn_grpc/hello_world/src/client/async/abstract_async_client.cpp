#include "abstract_async_client.h"
#include <thread>
#include <chrono>
#include "logger.h"
#include "utils.h"

AbstractAsyncClient::AbstractAsyncClient(int threadPoolSize):
    threadPoolSize(threadPoolSize),
    threadPool(threadPoolSize)
{}

void AbstractAsyncClient::addToken(void* token, std::shared_ptr<AsyncCallResponseProcessor> processor)
{
    std::lock_guard<std::mutex> lock(tokensLock);
    tokens.insert({ token, processor });
}

bool AbstractAsyncClient::isTokenExist(void* token)
{
    std::lock_guard<std::mutex> lock(tokensLock);
    return tokens.count(token);
}

void AbstractAsyncClient::removeToken(void* token)
{
    std::lock_guard<std::mutex> lock(tokensLock);
    tokens.erase(token);
}

void AbstractAsyncClient::removeAllTokens()
{
    std::lock_guard<std::mutex> lock(tokensLock);
    tokens.clear();
}

size_t AbstractAsyncClient::getTokenCount()
{
    std::lock_guard<std::mutex> lock(tokensLock);
    return tokens.size();
}

std::shared_ptr<AsyncCallResponseProcessor> AbstractAsyncClient::getProcesser(void* token)
{
    std::lock_guard<std::mutex> lock(tokensLock);
    return tokens[token];
}

void AbstractAsyncClient::asyncCompleteRpc()
{
    void* token;
    bool ok = false;
    while (true) {
        grpc::CompletionQueue::NextStatus nextStatus = cq.AsyncNext(&token, &ok, std::chrono::system_clock::now() + std::chrono::milliseconds(1000));
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

void AbstractAsyncClient::startThreadPool()
{
    for (int i = 0; i < threadPoolSize; i++) {
        boost::asio::post(threadPool, std::bind(&AbstractAsyncClient::asyncCompleteRpc, this));
    }
}

void AbstractAsyncClient::waitForComplete()
{
    threadPool.join();
}

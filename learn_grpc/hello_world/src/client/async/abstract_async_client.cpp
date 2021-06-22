#include "abstract_async_client.h"
#include <thread>
#include <chrono>

AbstractAsyncClient::AbstractAsyncClient(int threadPoolSize):
    threadPoolSize(threadPoolSize),
    threadPool(threadPoolSize)
{}

void AbstractAsyncClient::addToken(void* token)
{
    std::lock_guard<std::mutex> lock(tokensLock);
    tokens.insert(token);
}

bool AbstractAsyncClient::tokenExist(void* token)
{
    std::lock_guard<std::mutex> lock(tokensLock);
    return tokens.count(token);
}

void AbstractAsyncClient::removeToken(void* token)
{
    std::lock_guard<std::mutex> lock(tokensLock);
    tokens.erase(token);
}

void AbstractAsyncClient::startThreadPool()
{
    //for (int i = 0; i < threadPoolSize; i++) {
    //    boost::asio::post(threadPool, &AbstractAsyncClient::asyncCompleteRpc, this);
    //}
}

void AbstractAsyncClient::waitForComplete()
{
    threadPool.join();
}

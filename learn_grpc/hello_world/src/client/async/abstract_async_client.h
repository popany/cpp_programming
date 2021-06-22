#pragma once

#include <grpcpp/grpcpp.h>
#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>
#include <set>
#include <mutex>

class AbstractAsyncClient
{
    grpc::CompletionQueue cq;
    int threadPoolSize;
    boost::asio::thread_pool threadPool;
    std::set<void*> tokens;
    std::mutex tokensLock;
    virtual void asyncCompleteRpc() = 0;

    void addToken(void* token);
    bool tokenExist(void* token);
    void removeToken(void* token);
    bool isComplete();

protected:

    template<class F>
    auto prepareAsyncCall(F f, void* token) 
        -> typename std::result_of<F(grpc::CompletionQueue*)>::type
    {
        addToken(token);
        return f(&cq);
    }

public:
    AbstractAsyncClient(int threadPoolSize);
    void startThreadPool();
    void waitForComplete();
};

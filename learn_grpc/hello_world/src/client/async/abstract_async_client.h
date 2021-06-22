#pragma once

#include <grpcpp/grpcpp.h>
#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>
#include <map>
#include <mutex>

class AsyncCallResponseProcessor
{
public:
    virtual void process() = 0;
};

class AbstractAsyncClient
{
    grpc::CompletionQueue cq;
    int threadPoolSize;
    boost::asio::thread_pool threadPool;
    std::map<void*, std::shared_ptr<AsyncCallResponseProcessor>> tokens;
    std::mutex tokensLock;
    virtual void asyncCompleteRpc() = 0;

    void addToken(void* token, std::shared_ptr<AsyncCallResponseProcessor> processor);
    bool isTokenExist(void* token);
    void removeToken(void* token);
    bool isComplete();

protected:

    template<class F>
    auto prepareAsyncCall(F prepare, std::shared_ptr<AsyncCallResponseProcessor> processor) 
        -> typename std::result_of<F(grpc::CompletionQueue*)>::type
    {
        addToken(processor.get(), processor);
        return prepare(&cq);
    }

public:
    AbstractAsyncClient(int threadPoolSize);
    void startThreadPool();
    void waitForComplete();
};

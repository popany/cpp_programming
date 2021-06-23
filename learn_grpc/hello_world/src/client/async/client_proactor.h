#pragma once

#include <grpcpp/grpcpp.h>
#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>
#include <map>
#include <mutex>

class AsyncCallResponseProcessor
{
public:
    virtual void process(bool operationOk) = 0; 
    virtual bool isComplete() = 0;
};

class ClientProactor
{
    // 
    // Completion Queues and Threading in the Async API - https://grpc.github.io/grpc/cpp/md_doc_cpp_perf_notes.html
    // Right now, the best performance trade-off is having numcpu's threads and one completion queue per thread.
    grpc::CompletionQueue cq;
    int threadPoolSize;
    boost::asio::thread_pool threadPool;

    std::map<void*, std::shared_ptr<AsyncCallResponseProcessor>> tokens;
    std::mutex tokensLock;

    void addToken(void* token, std::shared_ptr<AsyncCallResponseProcessor> processor);
    bool isTokenExist(void* token);
    void removeToken(void* token);
    void removeAllTokens();
    size_t getTokenCount();
    std::shared_ptr<AsyncCallResponseProcessor> getProcesser(void* token);
    void asyncCompleteRpc();

    ClientProactor(int threadPoolSize);
public:
    ClientProactor(const ClientProactor&) = delete;
    void operator=(const ClientProactor&) = delete;

    static ClientProactor& getInstance();

    template<class F>
    auto prepareAsyncCall(F prepare, std::shared_ptr<AsyncCallResponseProcessor> processor) 
        -> typename std::result_of<F(grpc::CompletionQueue*)>::type
    {
        addToken(processor.get(), processor);
        return prepare(&cq);
    }

    void startThreadPool();
    void waitForComplete();
};

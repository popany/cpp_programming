#pragma once

#include <grpcpp/grpcpp.h>
#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>
#include <unordered_map>
#include <mutex>
#include <vector>
#include <stdexcept>
#include <sstream>
#include "event.h"
#include "event_handler_manager.h"

class ClientProactor
{
    // Completion Queues and Threading in the Async API - https://grpc.github.io/grpc/cpp/md_doc_cpp_perf_notes.html
    // Right now, the best performance trade-off is having numcpu's threads and one completion queue per thread.
    std::vector<grpc::CompletionQueue> cqs; // You can use only one cq for all the threads in the threadPool, but if this the case, you must avoid race conditions in your own functions
    int threadPoolSize;
    boost::asio::thread_pool threadPool;
    std::atomic_int32_t cqIdx;

    EventHandlerManager handlerManager;

    void asyncCompleteRpc(grpc::CompletionQueue& cq);

    ClientProactor(int threadPoolSize);
public:
    ClientProactor(const ClientProactor&) = delete;
    void operator=(const ClientProactor&) = delete;

    static ClientProactor& getInstance();

    template<class F>
    auto prepareAsyncCall(F prepare, std::shared_ptr<EventHandler> handler, event_key_t& key) 
        -> typename std::result_of<F(grpc::CompletionQueue*)>::type
    {
        key = handlerManager.add(handler).getKey();
        return prepare(&cqs[cqIdx++ % threadPoolSize]);
    }

    void startThreadPool();
    void waitForComplete();
};

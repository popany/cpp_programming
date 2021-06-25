#pragma once

#include <grpcpp/grpcpp.h>
#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>
#include <unordered_map>
#include <mutex>
#include <vector>
#include <stdexcept>
#include <sstream>

typedef uint32_t event_key_t;
typedef uint32_t event_opt_t;

enum EVENT_OPT
{
    START_CALL = 0,
    FINISH = 1,
    READ = 2,
    WRITE = 3,
    WRITE_DONE = 4,
};

class Event
{
    union {
        void* token;
        struct {
            event_key_t key;
            event_opt_t opt;
        };
    };

public:
    explicit Event(void* p) :
        token(p)
    {}

    explicit Event(event_key_t key) :
        key(key)
    {}

    void* getToken()
    {
        return token;
    }

    event_key_t getKey()
    {
        return key;
    }

    event_opt_t getOpt()
    {
        return opt;
    }

    void setOpt(event_opt_t opt)
    {
        this->opt = opt;
    }
};

static_assert(sizeof(Event) <= sizeof(void*));

class EventHandler
{
public:
    virtual void process(bool optOk, Event event) = 0; 
    virtual bool isComplete() = 0;
};

class EventHandlerManager
{
    std::unordered_map<event_key_t, std::shared_ptr<EventHandler>> handlers;
    event_key_t lastKey;
    std::mutex mtx;

public:
    EventHandlerManager() :
        lastKey(0)
    {}

    Event add(std::shared_ptr<EventHandler> handler)
    {
        std::lock_guard<std::mutex> lock(mtx);
        event_key_t old = lastKey;
        while (handlers.count(++lastKey) > 0) {
            if (lastKey == old) {
                throw std::runtime_error("no more space for new event handler");
            }
        }
        handlers.insert({ lastKey, handler });
        return Event(lastKey);
    }

    bool contains(event_key_t key)
    {
        std::lock_guard<std::mutex> lock(mtx);
        return handlers.count(key) > 0;
    }

    std::shared_ptr<EventHandler> get(event_key_t key)
    {
        std::lock_guard<std::mutex> lock(mtx);
        if (handlers.count(key) == 0) {
            throw std::runtime_error("failed to find eventHandler");
        }
        return handlers[key];
    }

    size_t count()
    {
        std::lock_guard<std::mutex> lock(mtx);
        return handlers.size();
    }

    void remove(event_key_t key)
    {
        std::lock_guard<std::mutex> lock(mtx);
         handlers.erase(key);
    }

    void clear()
    {
        std::lock_guard<std::mutex> lock(mtx);
        handlers.clear();
    }

    std::string getKeys() // for debug
    {
        std::stringstream ss;
        for (const auto& kv : handlers) {
            ss << kv.first << ",";
        }
        return ss.str();
    }
};

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

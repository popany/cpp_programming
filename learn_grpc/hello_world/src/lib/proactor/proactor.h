#pragma once

#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>
#include <vector>
#include <atomic>
#include "event_handler_manager.h"

class Proactor
{
    boost::asio::thread_pool threadPool;

    virtual void demultiplex() = 0;

protected:
    int threadPoolSize;
    EventHandlerManager handlerManager;

public:
    Proactor(int threadPoolSize);

    void startDemuxer();
    void waitForComplete();
};

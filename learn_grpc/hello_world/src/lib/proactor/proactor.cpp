#include "proactor.h"
#include <functional>

Proactor::Proactor(int threadPoolSize) :
    threadPoolSize(threadPoolSize),
    threadPool(threadPoolSize)
{}

void Proactor::startDemuxer()
{
    for (int i = 0; i < threadPoolSize; i++) {
        boost::asio::post(threadPool, std::bind(&Proactor::demultiplex, this));
    }
}

void Proactor::waitForComplete()
{
    threadPool.join();
}

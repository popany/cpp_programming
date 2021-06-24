#pragma once

#include <condition_variable>
#include <mutex>
#include <atomic>
#include <stdexcept>
#include <iostream>

namespace utils
{

class Semaphore
{
    const int maxCount;
    std::atomic_int count;
    std::condition_variable cv;
    std::mutex mtx;

public:
    Semaphore(int count):
        count(count),
        maxCount(count)
    {}

    void acquire()   
    {
        int expected = count.load();
        while (!count.compare_exchange_weak(expected, expected - 1)) {
        }
        if (expected <= 0) {
            std::unique_lock<std::mutex> lk(mtx);
            cv.wait(lk, [&] { return count >= 0; });
        }
    }

    void release()
    {
        int expected = count.load();
        while (expected < maxCount && !count.compare_exchange_weak(expected, expected + 1)) {
        }
        if (expected <= 0) {
            std::unique_lock<std::mutex> lk(mtx);
            cv.notify_one();
        }
    }

};

}

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
    const int capacity;
    std::atomic_int remaining;
    std::condition_variable cv;
    std::mutex mtx;
    std::atomic_int waited;

public:
    Semaphore(int capacity):
        capacity(capacity),
        remaining(capacity),
        waited(0)
    {}

    void acquire()
    {
        while (true) {
            int expected = remaining.load();
            while (expected > 0 && !remaining.compare_exchange_weak(expected, expected - 1)) {
            }
            if (expected == 0) {
                std::unique_lock<std::mutex> lk(mtx);
                waited++;
                cv.wait(lk, [&] { return remaining > 0; });
                waited--;
            }
            else {
                return;
            }
        }
    }

    bool release()
    {
        int expected = remaining.load();
        while (expected < capacity && !remaining.compare_exchange_weak(expected, expected + 1)) {
        }
        if (expected == capacity) {
            return false;  // means the release option has no effect, i.e. the release option is lost
        }
        if (waited > 0) {  // there can be thread going to wait even if `waited` == 0, but if this is the case, `remaining > 0` in `cv.wait` makes the thread not blocked
            std::unique_lock<std::mutex> lk(mtx);
            cv.notify_one();
        }
        return true;
    }

};

}

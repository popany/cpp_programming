#include <iostream>
#include <thread>
#include <chrono>
#include <stdexcept>
#include <mutex>
#include <condition_variable>

void DetachNotJoinableThread1()
{
    std::mutex m;
    std::condition_variable cv;
    bool ready = false;
    try {
        std::thread t([&]() {
            std::cout << "run 1" << std::endl;
            {
                std::unique_lock<std::mutex> lk(m);
                ready = true;
            }
            cv.notify_one();
        });
        std::thread t2(std::move(t));
        t2.detach();
        {
            std::unique_lock<std::mutex> lk(m);
            cv.wait(lk, [&]{ return ready; });
        }
        t.detach();
    } catch (const std::system_error &e) {
        std::cout << "[" << __FUNCTION__ << "] exception: " << e.what() << std::endl;
    }
}

void DetachNotJoinableThread2()
{
    try {
        std::thread t;
        t.detach();
    } catch (const std::system_error &e) {
        std::cout << "[" << __FUNCTION__ << "] exception: " << e.what() << std::endl;
    }
}

void CallThreadDetach(std::thread& t, std::mutex& m, std::condition_variable& cv, bool& ready)
{
    try {
        t.detach();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        {
            std::unique_lock<std::mutex> lk(m);
            ready = true;
        }
        cv.notify_one();
    } catch (const std::system_error &e) {
        std::cout << "[" << __FUNCTION__ << "] exception: " << e.what() << std::endl;
    }
}

void DetachSelf()
{
    try {
        std::mutex m;
        std::condition_variable cv;
        bool ready = false;
        std::thread t;
        t = std::thread([&](){
            CallThreadDetach(t, m, cv, ready);
        });
        {
            std::unique_lock<std::mutex> lk(m);
            cv.wait(lk, [&]{ return ready; });
        }
    } catch (const std::system_error &e) {
        std::cout << "[" << __FUNCTION__ << "] exception: " << e.what() << std::endl;
    }
}

int main()
{
    DetachNotJoinableThread1(); // exception: Invalid argument
    DetachNotJoinableThread2(); // exception: Invalid argument
    DetachSelf();

    return 0;
}

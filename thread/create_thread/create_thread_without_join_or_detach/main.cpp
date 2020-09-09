#include <iostream>
#include <thread>
#include <string>
#include <chrono>
#include <condition_variable>
#include <mutex>

void Run(std::condition_variable& cv, std::mutex& m, bool& ready)
{
    std::thread t([&]{
        for (int i = 0; i < 10; i++) {
            std::cout << "i: " << std::to_string(i) << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        {
            std::lock_guard<std::mutex> lk(m);
            ready = true;
        }
        cv.notify_one();
    });
    std::cout << "Run() end" << std::endl;
}

int main()
{
    std::condition_variable cv;
    std::mutex m;
    bool ready = false;
    Run(cv, m, ready); // Program terminated with signal SIGABRT, Aborted.
    {
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, [&] { return ready; });
    }

    return 0;
}


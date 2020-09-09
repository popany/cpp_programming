#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

void Run()
{
    std::mutex m;
    std::condition_variable cv;
    int step = 0;

    std::thread t = std::thread([&]() {
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            std::unique_lock<std::mutex> lk(m);
            cv.wait(lk, [&]{ return step == 1; });
        }

        std::cout << "step: 1" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        {
            std::unique_lock<std::mutex> lk(m);
            step = 2;
        }
        cv.notify_one();
        
    });
    t.detach();

    {
        std::unique_lock<std::mutex> lk(m);
        step = 1;
    }
    cv.notify_one();
    
    {
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, [&]{ return step == 2; });
    }
    std::cout << "step: 2" << std::endl;
}

int main()
{
    Run();

    return 0;
}


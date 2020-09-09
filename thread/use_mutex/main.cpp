#include <thread>
#include <iostream>
#include <mutex>
#include <chrono>
#include <vector>
#include <random>

void Func(int& v, const int idx, std::mutex& m)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(10, 50);
    for (int i = 0; i < 5; ++i) {
        {
            std::unique_lock<std::mutex> lk(m);
            std::cout << "thread_id: " << std::this_thread::get_id() << ", idx: " << idx << ", v:" << v++ << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(distrib(gen))); 
    }
}

void Run()
{
    std::mutex m;
    int v = 0;
    std::vector<std::thread> t;
    for (int i = 0; i < 10; i++) {
        t.push_back(std::thread(Func, std::ref(v), i, std::ref(m)));
    }
    for (int i = 0; i < t.size(); ++i) {
        t[i].join();
    }
}

int main()
{
    Run();

    return 0;
}


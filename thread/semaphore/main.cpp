#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <stdexcept>
#include <chrono>
#include <functional>

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

void Test1()
{
	Semaphore s(1);
	std::atomic_int a{ 0 };
	int n = 1000;

	std::function<void(int)> f = [&] (int id) {
		for (int i = 0; i < n; i++) {
			s.acquire();
            std::cout << id << " - acquire" << std::endl;
			int x = a.load();
			if (x != 0) {
				std::cout << "error, x = " << x << std::endl;
			}
			a++;
			//std::this_thread::sleep_for(std::chrono::milliseconds(1));
			a--;
            std::cout << id << " - release" << std::endl;
			s.release();
		}
	};

	std::thread t1(f, 1);
	std::thread t2(f, 2);
	t1.join();
    t2.join();
}

int main()
{
	Test1();

    return 0;
}

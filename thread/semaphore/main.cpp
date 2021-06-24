#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <stdexcept>
#include <chrono>
#include <functional>
#include <vector>

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
        while (true) {
            int expected = count.load();
            while (expected > 0 && !count.compare_exchange_weak(expected, expected - 1)) {
            }
            if (expected == 0) {
                std::unique_lock<std::mutex> lk(mtx);
                cv.wait(lk, [&] { return count > 0; });
            }
            else {
                return;
            }
        }
    }

    void release()
    {
        int expected = count.load();
        while (expected < maxCount && !count.compare_exchange_weak(expected, expected + 1)) {
        }
        std::unique_lock<std::mutex> lk(mtx);
        cv.notify_one();
    }

};

void Test(int sn, int tn, int pn)
{
	Semaphore s(sn);
	std::atomic_int a{ 0 };

	std::function<void(int)> f = [&] (int id) {
		for (int i = 0; i < pn; i++) {
			s.acquire();

            std::cout << id << " - acquire" << std::endl;

			a++;
			int x = a.load();
			if (x > sn) {
				std::cout << "error, x = " << x << std::endl;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			a--;

            std::cout << id << " - release" << std::endl;

			s.release();
		}
	};

    std::vector<std::thread> v;
    v.reserve(tn);
    for (int i = 0; i <  tn; i++) {
        v.emplace_back(std::thread(f, i));
    }

    for (auto& t : v) {
        t.join();
    }
}

int main(int argc, char* argv[])
{
	Test(1, 2, 100);

    return 0;
}

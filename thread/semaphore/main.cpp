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

void Test(int sn, int atn, int rtn, int pn)
{
	Semaphore s(sn);
	std::atomic_int a{ 0 };
    std::atomic_int apn{ pn };
    std::atomic_int rpn{ pn };

	std::function<void(int)> aq = [&] (int id) {
		while (apn) {
			s.acquire();

			a++;
            apn--;
            std::cout << id << std::endl;

			int x = a.load();
			if (x > sn) {
				std::cout << "error, x = " << x << std::endl;
                throw std::runtime_error("");
			}
		}
	};

    std::function<void(int)> rl = [&] (int id) {
		while (rpn) {
            int e = a.load();
            while (e > 0 && !a.compare_exchange_weak(e, e - 1)) {
            }
            if (e <= 0) {
                // std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
            }

            rpn--;

			s.release();
		}
	};

    std::vector<std::thread> av;
    av.reserve(atn);
    for (int i = 0; i <  atn; i++) {
        av.emplace_back(std::thread(aq, i));
    }

    std::vector<std::thread> rv;
    rv.reserve(rtn);
    for (int i = 0; i <  rtn; i++) {
        rv.emplace_back(std::thread(rl, i));
    }

    for (int i = 0; i < atn; i++) {
        av[i].join();
    }

    for (int i = 0; i < rtn; i++) {
        rv[i].join();
    }
}

int main(int argc, char* argv[])
{
	Test(1, 2, 2, 1000);

    return 0;
}


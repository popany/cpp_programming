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
            // std::cout << id << std::endl;

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
    std::cout << "1, 1, 1, 10000" << std::endl;
	Test(1, 1, 1, 10000);
    std::cout << "1, 2, 1, 10000" << std::endl;
	Test(1, 2, 1, 10000);
    std::cout << "1, 1, 2, 10000" << std::endl;
	Test(1, 1, 2, 10000);
    std::cout << "1, 5, 3, 10000" << std::endl;
	Test(1, 5, 3, 10000);
    std::cout << "1, 3, 5, 10000" << std::endl;
	Test(1, 3, 5, 10000);

    std::cout << "2, 1, 1, 10000" << std::endl;
	Test(2, 1, 1, 10000);
    std::cout << "2, 2, 1, 10000" << std::endl;
	Test(2, 2, 1, 10000);
    std::cout << "2, 1, 2, 10000" << std::endl;
	Test(2, 1, 2, 10000);
    std::cout << "2, 5, 3, 10000" << std::endl;
	Test(2, 5, 3, 10000);
    std::cout << "2, 3, 5, 10000" << std::endl;
	Test(2, 3, 5, 10000);

    std::cout << "3, 1, 1, 10000" << std::endl;
	Test(3, 1, 1, 10000);
    std::cout << "3, 2, 1, 10000" << std::endl;
	Test(3, 2, 1, 10000);
    std::cout << "3, 1, 2, 10000" << std::endl;
	Test(3, 1, 2, 10000);
    std::cout << "3, 5, 3, 10000" << std::endl;
	Test(3, 5, 3, 10000);
    std::cout << "3, 3, 5, 10000" << std::endl;
	Test(3, 3, 5, 10000);

    std::cout << "5, 1, 1, 10000" << std::endl;
	Test(5, 1, 1, 10000);
    std::cout << "5, 2, 1, 10000" << std::endl;
	Test(5, 2, 1, 10000);
    std::cout << "5, 1, 2, 10000" << std::endl;
	Test(5, 1, 2, 10000);
    std::cout << "5, 5, 3, 10000" << std::endl;
	Test(5, 5, 3, 10000);
    std::cout << "5, 3, 5, 10000" << std::endl;
	Test(5, 3, 5, 10000);

    return 0;
}


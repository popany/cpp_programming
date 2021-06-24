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

    bool release()
    {
        int expected = count.load();
        while (expected < maxCount && !count.compare_exchange_weak(expected, expected + 1)) {
        }
        if (expected == maxCount) {
            return false;  // means the release option has no effect, i.e. the release option is lost
        }
        if (expected == 0) {  // bug
            std::unique_lock<std::mutex> lk(mtx);
            cv.notify_one();
        }
        return true;
    }

};

void Test(int sn, int atn, int rtn, int count)
{
	Semaphore s(sn);
	std::atomic_int number{ 0 };
    std::atomic_int acquireCount{ 0 };
    std::atomic_int releaseCount{ 0 };

	std::function<void(int)> aq = [&] (int id) {
		while (true) {
            int ac = acquireCount.load();
            while (ac < count && !acquireCount.compare_exchange_weak(ac, ac + 1)) {
            }
            if (ac == count) {
                break;
            }

			s.acquire();

			number++;
			int x = number.load();
			if (x > sn) {
				std::cout << "error, x = " << x << std::endl;
                throw std::runtime_error("");
			}
		}
	};

    std::function<void(int)> rl = [&] (int id) {
		while (true) {
            // std::this_thread::sleep_for(std::chrono::milliseconds(10));
            
            int ac = acquireCount.load();
            int rc = releaseCount.load();
            while (rc < ac - sn && !releaseCount.compare_exchange_weak(rc, rc + 1)) {
            }
            if (rc == count - sn) {
                break;
            }
            if (rc >= ac - sn) {  // ac is not a fix value, so, rc can > ac - sn
                continue;
            }
            
            number--;
			while (!s.release()) {  // make sure the release option will not be lost
            }
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


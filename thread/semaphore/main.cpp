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

void TestA(int sn, int atn, int rtn, int count)
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

			int x = number.fetch_add(1);
			if (x >= sn) {
				std::cout << "error, acquire, x = " << x << std::endl;
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

void Test1()
{
    std::cout << "1, 1, 1, 10000" << std::endl;
	TestA(1, 1, 1, 10000);
    std::cout << "1, 2, 1, 10000" << std::endl;
	TestA(1, 2, 1, 10000);
    std::cout << "1, 1, 2, 10000" << std::endl;
	TestA(1, 1, 2, 10000);
    std::cout << "1, 5, 3, 10000" << std::endl;
	TestA(1, 5, 3, 10000);
    std::cout << "1, 3, 5, 10000" << std::endl;
	TestA(1, 3, 5, 10000);

    std::cout << "2, 1, 1, 10000" << std::endl;
	TestA(2, 1, 1, 10000);
    std::cout << "2, 2, 1, 10000" << std::endl;
	TestA(2, 2, 1, 10000);
    std::cout << "2, 1, 2, 10000" << std::endl;
	TestA(2, 1, 2, 10000);
    std::cout << "2, 5, 3, 10000" << std::endl;
	TestA(2, 5, 3, 10000);
    std::cout << "2, 3, 5, 10000" << std::endl;
	TestA(2, 3, 5, 10000);

    std::cout << "3, 1, 1, 10000" << std::endl;
	TestA(3, 1, 1, 10000);
    std::cout << "3, 2, 1, 10000" << std::endl;
	TestA(3, 2, 1, 10000);
    std::cout << "3, 1, 2, 10000" << std::endl;
	TestA(3, 1, 2, 10000);
    std::cout << "3, 5, 3, 10000" << std::endl;
	TestA(3, 5, 3, 10000);
    std::cout << "3, 3, 5, 10000" << std::endl;
	TestA(3, 3, 5, 10000);

    std::cout << "5, 1, 1, 10000" << std::endl;
	TestA(5, 1, 1, 10000);
    std::cout << "5, 2, 1, 10000" << std::endl;
	TestA(5, 2, 1, 10000);
    std::cout << "5, 1, 2, 10000" << std::endl;
	TestA(5, 1, 2, 10000);
    std::cout << "5, 5, 3, 10000" << std::endl;
	TestA(5, 5, 3, 10000);
    std::cout << "5, 3, 5, 10000" << std::endl;
	TestA(5, 3, 5, 10000);
}

void TestB(int sn, int atn)
{
	Semaphore s(sn);
    std::atomic_int ac;

	std::function<void(int)> aq = [&] (int id) {
        ac++;
        s.acquire();
	};

    std::vector<std::thread> av;
    av.reserve(atn);
    for (int i = 0; i <  atn; i++) {
        av.emplace_back(std::thread(aq, i));
    }

    while (ac < atn) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));  // wait until all ac thread blocked

    for (int i = 0; i <  atn - sn; i++) {
        while (!s.release()) {
        }
    }

    for (int i = 0; i < atn; i++) {
        av[i].join();
    }
}

void Test2()
{
    std::cout << "TestB - 1, 1" << std::endl;
    TestB(1, 1);
    std::cout << "TestB - 1, 2" << std::endl;
    TestB(1, 2);
    std::cout << "TestB - 1, 3" << std::endl;
    TestB(1, 3);
    std::cout << "TestB - 1, 8" << std::endl;
    TestB(1, 8);

    std::cout << "TestB - 2, 1" << std::endl;
    TestB(2, 1);
    std::cout << "TestB - 2, 2" << std::endl;
    TestB(2, 2);
    std::cout << "TestB - 2, 3" << std::endl;
    TestB(2, 3);
    std::cout << "TestB - 2, 8" << std::endl;
    TestB(2, 8);

    std::cout << "TestB - 3, 1" << std::endl;
    TestB(3, 1);
    std::cout << "TestB - 3, 2" << std::endl;
    TestB(3, 2);
    std::cout << "TestB - 3, 3" << std::endl;
    TestB(3, 3);
    std::cout << "TestB - 3, 8" << std::endl;
    TestB(3, 8);

    std::cout << "TestB - 1, 8" << std::endl;
    TestB(1, 8);
    std::cout << "TestB - 1, 16" << std::endl;
    TestB(1, 16);
    std::cout << "TestB - 1, 32" << std::endl;
    TestB(1, 32);
    std::cout << "TestB - 1, 64" << std::endl;
    TestB(1, 64);

    std::cout << "TestB - 3, 8" << std::endl;
    TestB(3, 8);
    std::cout << "TestB - 3, 16" << std::endl;
    TestB(3, 16);
    std::cout << "TestB - 3, 32" << std::endl;
    TestB(3, 32);
    std::cout << "TestB - 3, 64" << std::endl;
    TestB(3, 64);

}

int main(int argc, char* argv[])
{
    Test1();
    Test2();
    return 0;
}


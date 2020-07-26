#include <iostream>
#include <thread>
#include <future>

void UseFutureToGetValue()
{
    std::promise<int> p;
    std::future<int> f = p.get_future();
    std::thread t([&](){
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        p.set_value(1);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    });
    std::cout << "[" << __FUNCTION__  << "] " << f.get() << std::endl;
    t.join();
}

void UseFutureToWait()
{
    std::promise<void> p;
    std::future<void> f = p.get_future();
    std::thread t([&](){
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        p.set_value();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    });
    f.wait();
    std::cout << "[" << __FUNCTION__  << "]" << std::endl;
    t.join();
}

void PromiseToThrow(std::promise<int> p)
{
    try {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        throw std::runtime_error("error in PromiseToThrow");
    } catch (...) {
        try {
            p.set_exception(std::current_exception());
        } catch (...) {
        }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

void UseFutureToHandleException()
{
    std::promise<int> p;
    std::future<int> f = p.get_future();
    std::thread t(PromiseToThrow, std::move(p));

    std::cout << "[" << __FUNCTION__ << "] valid: " << f.valid() << std::endl;
    try {
        f.get();
    } catch (const std::exception& e) {
        std::cout << "[" << __FUNCTION__ << "] exception: " << e.what() << std::endl;
    }
    t.join();
    std::cout << "[" << __FUNCTION__ << "] valid: " << f.valid() << std::endl;
}

void UseFutureToHandleException2()
{
    std::promise<int> p;
    std::future<int> f = p.get_future();
    std::thread t(PromiseToThrow, std::move(p));
    t.join();

    std::cout << "[" << __FUNCTION__ << "] valid: " << f.valid() << std::endl;
    try {
        f.get();
    } catch (const std::exception& e) {
        std::cout << "[" << __FUNCTION__ << "] exception: " << e.what() << std::endl;
    }
    std::cout << "[" << __FUNCTION__ << "] valid: " << f.valid() << std::endl;

    try {
        f.get();
    } catch (const std::exception& e) {
        std::cout << "[" << __FUNCTION__ << "] exception: " << e.what() << std::endl;
    }
}

void Run()
{
    UseFutureToGetValue();
    UseFutureToWait();
    UseFutureToHandleException();
    UseFutureToHandleException2();
}

int main()
{
    Run();

    return 0;
}

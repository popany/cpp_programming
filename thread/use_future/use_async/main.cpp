#include <iostream>
#include <future>

void UseAsync()
{
    std::future<int> f = std::async([]{
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        return 1;
    });   

    std::cout << "[" << __FUNCTION__ << "] " << f.get() << std::endl;
}

void UseAsyncHandleException()
{
    std::future<int> f = std::async([]{
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        throw std::runtime_error("async runtime_error");
        return 1;
    });   

    try {
        f.get();
    } catch (const std::exception& e) {
        std::cout << "[" << __FUNCTION__ << "] exception: " << e.what() << std::endl;
    }
}

void Run()
{
    UseAsync();
    UseAsyncHandleException();
}

int main()
{
    Run();

    return 0;
}


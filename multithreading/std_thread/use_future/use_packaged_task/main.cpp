#include <iostream>
#include <future>

void UsePackagedTask()
{
    std::packaged_task<int(int)> t([](int x){
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        return x;
    });   
    std::future<int> f = t.get_future();
    t(1);

    std::cout << "[" << __FUNCTION__ << "] " << f.get() << std::endl;
}

void UsePackagedTaskHandleException()
{
    std::packaged_task<int(int)> t([](int x){
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        throw std::runtime_error("packaged_task error");
        return x;
    });   
    std::future<int> f = t.get_future();
    t(1);

    try {
        f.get();
    } catch (const std::runtime_error& e) {
        std::cout << "[" << __FUNCTION__ << "] exception: " << e.what() << std::endl;
    }
}

void Run()
{
    UsePackagedTask();
    UsePackagedTaskHandleException();
}

int main()
{
    Run();

    return 0;
}


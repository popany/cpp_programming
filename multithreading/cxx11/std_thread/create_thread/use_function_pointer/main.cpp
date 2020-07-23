#include <iostream>
#include <thread>
#include <string>

void Func()
{
    std::cout << "Hello Word" << std::endl;
}

void Run()
{
    std::thread t(Func);
    t.join();
}

int main()
{
    Run();

    return 0;
}


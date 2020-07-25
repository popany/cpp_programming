#include <iostream>
#include <thread>
#include <string>

void Func()
{
    std::cout << "Hello World" << std::endl;
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


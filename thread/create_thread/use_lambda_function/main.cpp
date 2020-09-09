#include <iostream>
#include <thread>
#include <string>

void Run()
{
    std::thread t([]{
        std::cout << "Hello World" << std::endl;
    });
    t.join();
}

int main()
{
    Run();

    return 0;
}


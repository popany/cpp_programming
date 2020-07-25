#include <iostream>
#include <thread>
#include <string>

class F
{
public:
    F() {}

    void operator() ()
    {
        std::cout << "Hello World" << std::endl;
    }
};


void Run()
{
    F func;
    std::thread t(func);
    t.join();
}

int main()
{
    Run();

    return 0;
}


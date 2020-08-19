#include <iostream>
#include <memory>

class A
{
public:
    A()
    {
        std::cout << "A()" << std::endl;
    }

    ~A()
    {
        std::cout << "~A()" << std::endl;
    }
};

void Run()
{
    std::shared_ptr<A> a = std::make_shared<A>();
}

int main()
{
    Run();

    return 0;
}

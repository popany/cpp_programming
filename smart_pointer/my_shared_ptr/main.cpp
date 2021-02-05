#include "my_shared_ptr.h"
#include <iostream>
#include <string>

class A
{
public:
    std::string name;

    A(std::string name): name(name)
    {
        std::cout << "A(), " << name << std::endl;
    }

    ~A()
    {
        std::cout << "~A(), " << name << std::endl;
    }

};

void run()
{
    MySharedPtr<A> p1 = new A("1");
    MySharedPtr<A> p2 = p1;
    MySharedPtr<A> p3 = new A("2");
    p3 = p2;
}

int main()
{
    run();

    return 0;
}

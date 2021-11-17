#include <iostream>

class A
{
    int a{ 1 };

    void print()
    {
        std::cout << "A::print()" << std::endl;
    }

public:

    void func()
    {
        auto f = [&] {  // catch by reference
            std::cout << a << std::endl;  // lambda expression defined in member function can access private members

            print();  // lambda expression defined in member function can call private member functions

            this->print();  // lambda expression defined in member function can use this pointer
        };

        f();
    }
};

int main()
{
    A a;
    a.func();

    return 0;
}


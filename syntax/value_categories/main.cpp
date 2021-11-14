#include <iostream>

void func(const int& a)
{
    std::cout << "func(const int& a)" << std::endl;
}

void func(int&& a)
{
    std::cout << "func(int&& a)" << std::endl;
}

int& foo()
{
    static int a;
    return a;
}

int&& bar()
{
    static int a;
    return static_cast<int&&>(a);
}

int main()
{
    func(foo());
    func(bar());  // bar() is a xvalue expression, for it's a function call whose return type is rvalue reference to object

    return 0;
}

// output:
// func(const int& a)
// func(int&& a)


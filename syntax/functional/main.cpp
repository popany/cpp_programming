#include <iostream>
#include <functional>
#include <memory>

struct S
{
    int a[100];
};

void func()
{
    S s;
    auto f_lambda = [=] {
        std::cout << &s << std::endl;
    };
    auto f_bind = std::bind(f_lambda);
    auto f_function = std::function<void()>(f_lambda);

    f_lambda();  // print address on stack
    f_bind();  // print address on stack
    f_function();  // print address on heap
}

int main()
{
    func();

    return 0;
}

#include <iostream>
#include <functional>
#include <memory>

void call(const std::function<void()>& f)
{
    f();
}

void func()
{
    int a = 0;
    auto f = [=] {
        std::cout << &a << std::endl;
    };
    auto f2 = std::make_shared<std::function<void()>>(f);

    std::cout << &a << std::endl;  // print address on stack
    f();  // print address on stack
    call(f);  // print address on stack
    call(std::bind(f));  // print address on heap
    call(*f2);  // print address on heap
}

int main()
{
    func();

    return 0;
}


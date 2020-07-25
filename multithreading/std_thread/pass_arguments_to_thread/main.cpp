#include <thread>
#include <iostream>
#include <stdexcept>
#include <string>

void PassByReferenceLambdaCaptureReference()
{
    int a = 0;
    std::thread t([&]() {
        a++;
    });
    t.join();
    std::cout << "[" << __FUNCTION__ << "] a: " << a << std::endl;
}

void PassByConstReference()
{
    int a = 0;
    std::thread t([](const int &x) {
        int& y = const_cast<int&>(x);
        y++;
    }, a);
    t.join();
    std::cout << "[" << __FUNCTION__ << "] a: " << a << std::endl;
}

void PassByConstReferenceUseStdRef()
{
    int a = 0;
    std::thread t([](const int &x) {
        int& y = const_cast<int&>(x);
        y++;
    }, std::ref(a));
    t.join();
    std::cout << "[" << __FUNCTION__ << "] a: " << a << std::endl;
}

void PassByReferenceUseStdRef()
{
    int a = 0;
    std::thread t([](int &x) {
        x++;
    }, std::ref(a));
    t.join();
    std::cout << "[" << __FUNCTION__ << "] a: " << a << std::endl;
}

class A
{
    static constexpr int a = 1;
public:
    A() {}
    
    void Func(const std::string s)
    {
        std::cout << "[" << __FUNCTION__ << "] " << s << " " << a << std::endl;
    }
};

void CallMemberFunction()
{
    A a;
    std::thread t(&A::Func, &a, "a");
    t.join();
}

void Run()
{
    PassByReferenceLambdaCaptureReference(); // a: 1
    PassByConstReference(); // a: 0
    PassByConstReferenceUseStdRef(); // a: 1
    PassByReferenceUseStdRef(); // a: 1
    CallMemberFunction();
}

int main()
{
    Run();

    return 0;
}

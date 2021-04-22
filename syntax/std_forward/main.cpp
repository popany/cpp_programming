#include <iostream>
#include <utility>

void Foo(int& a)
{
    std::cout << "Foo(int& a)" << std::endl;
}

void Foo(int&& a)
{
    std::cout << "Foo(int&& a)" << std::endl;
}

void Foo(const int& a)
{
    std::cout << "Foo(const int& a)" << std::endl;
}

template <typename T>
void Bar(T&& a)
{
    Foo(a);
}

template <typename T>
void Baz(T&& a)
{
    Foo(std::forward<T>(a));
}


int main()
{
    Bar(42);
    Baz(42);

    int x = 42;
    Bar(x);
    Baz(x);

    const int y = 42;
    Bar(y);
    Baz(y);

    Bar(std::move(x));
    Baz(std::move(x));

    return 0;
}



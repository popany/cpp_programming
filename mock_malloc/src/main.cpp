#include <stdlib.h>
#include <memory>
#include <iostream>

void CallMalloc()
{
    std::cout << __func__ << std::endl;

    char* p = (char*)malloc(100);
    free(p);
}

void CallNew()
{
    std::cout << __func__ << std::endl;

    char* p = new char[100];
    delete p;
}

void Func()
{
    CallMalloc();
    CallNew();
}

int main()
{
    Func();

    return 0;
}


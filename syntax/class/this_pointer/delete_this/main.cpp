#include <iostream>
#include <string>

class A
{
    std::string s;

public:
    A(std::string s):
        s(s)
    {}

    ~A()
    {
        std::cout << "~A()" << std::endl;
    }

    void PrintString()
    {
        std::cout << "A::PrintString(), s: " << s << std::endl;
    }

    void PrintX(std::string x)
    {
        std::cout << "A::PrintX(), x: " << x << std::endl;
    }

    void Release()
    {
        std::cout << "A::Release()" << std::endl;
        delete this;
    }
};

void Run()
{
    A* a = new A("sss");
    a->PrintString();
    a->Release();
    a->PrintX("xxx");
    a->PrintString();
}

int main()
{
    Run();

    return 0;
}

#include <iostream>

class B
{
public:
    virtual void func()
    {
        std::cout << "B::func()" << std::endl;
    }
};

class D : public B
{
public:
    void func() override
    {
        std::cout << "D::func()" << std::endl;
    }
};

int main()
{
    B b;
    D d;
    B* p = &d;
    void(B::*f)() = &B::func;

    typedef void(B::*FB)();
    union  // cannot cast void* to member function pointer directly, use union to work around
    {
        FB f;
        void* p;
    } fb, fd;
    fb.p = (void*)*(size_t*)*(size_t*)&b;
    fd.p = (void*)*(size_t*)*(size_t*)&d;

    p->func();     // D::func()
    p->B::func();  // B::func()
    (p->*f)();     // D::func()
    (p->*fb.f)();  // B::func()
    (p->*fd.f)();  // D::func()

    std::cout << "vtalbe address of B: " << (size_t*)*(size_t*)&b << std::endl;  // vtable is in the .rodata segment
    std::cout << "vtalbe address of D: " << (size_t*)*(size_t*)&d << std::endl;

    return 0;
}


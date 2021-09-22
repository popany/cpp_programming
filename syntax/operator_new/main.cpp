#include <iostream>
#include <stdlib.h>

void* operator new(size_t size)
{
    void *p = malloc(size);
    std::cout << "operator new: " << p << ", size: " << size << std::endl;
    return p;
}

void operator delete(void* p)
{
    std::cout << "operator delete: " << p << std::endl;
    free(p);
}

void* operator new[](size_t size)
{
    void *p = malloc(size);
    std::cout << "operator new[]: " << p << ", size: " << size << std::endl;
    return p;
}

void operator delete[](void* p)
{
    std::cout << "operator delete[]: " << p << std::endl;
    free(p);
}

class A
{
public:
    int a;
    A()
    {
        std::cout << "A()" << std::endl;
    }

    ~A()
    {
        std::cout << "~A()" << std::endl;
    }
};

class B
{
public:
    int a;
    int b;

    B()
    {
        std::cout << "B()" << std::endl;
    }

    ~B()
    {
        std::cout << "~B()" << std::endl;
    }

    void* operator new(size_t size)
    {
        void *p = malloc(size);
        std::cout << "B::operator new: " << p << ", size: " << size << std::endl;
        return p;
    }

    void operator delete(void* p)
    {
        std::cout << "B::operator delete: " << p << std::endl;
        free(p);
    }

    void* operator new[](size_t size)
    {
        void *p = malloc(size);
        std::cout << "B::operator new[]: " << p << ", size: " << size << std::endl;
        return p;
    }

    void operator delete[](void* p)
    {
        std::cout << "B::operator delete[]: " << p << std::endl;
        free(p);
    }


    void* operator new(size_t size, void* p)
    {
        std::cout << "B::Placement new: " << p << ", size: " << size << std::endl;
        return p;
    }

    void operator delete(void* p, void*)
    {
        std::cout << "B::Placement delete: " << p << std::endl;
    }

    void* operator new[](size_t size, void* p)
    {
        std::cout << "B::Placement new[]: " << p << ", size: " << size << std::endl;
        return p;
    }

    void operator delete[](void* p, void*)
    {
        std::cout << "B::Placement delete[]: " << p << std::endl;
    }
};

class C
{
public:
    int c;

    C()
    {
        std::cout << "C()" << std::endl;
        throw 1;
    }

    ~C()
    {
        std::cout << "~C()" << std::endl;
    }

    void* operator new(size_t size, void* p)
    {
        std::cout << "C::Placement new: " << p << ", size: " << size << std::endl;
        return p;
    }

    void operator delete(void* p, void*)
    {
        std::cout << "C::Placement delete: " << p << std::endl;
    }

    void* operator new[](size_t size, void* p)
    {
        std::cout << "C::Placement new[]: " << p << ", size: " << size << std::endl;
        return p;
    }

    void operator delete[](void* p, void*)
    {
        std::cout << "C::Placement delete[]: " << p << std::endl;
    }
};

template <typename T>
void CallNew()
{
    T* p = new T();
    delete p;
}

template <typename T>
void CallNewArray(size_t size)
{
    T* p = new T[size];
    delete[] p;
}

template <typename T>
void CallPlacementNew()
{
    void *buf = malloc(sizeof(T));
    try {
        T* p = new (buf)T();
        p->~T();
    } catch (...) {
        std::cout << "exception" << std::endl;
    }
    free(buf);
}

template <typename T>
void CallPlacementNewArray(size_t size)
{
    void *buf = malloc(sizeof(T) * size);
    try {
        T* p = new (buf)T[size];
        for (int i = 0; i < size; ++i) {
            p->~T();
        }
    } catch (...) {
        std::cout << "exception" << std::endl;
    }
    free(buf);
}

int main(int argc, char* argv[])
{
    std::cout << "------------------------------------------------------" << std::endl;
    std::cout << "> operator new" << std::endl;
    std::cout << std::endl;
    CallNew<A>();  // call operator new, operator delete
    std::cout << std::endl;
    CallNew<B>();  // call B::operator new, B::operator delete
    std::cout << std::endl;

    std::cout << "------------------------------------------------------" << std::endl;
    std::cout << "> operator new[]" << std::endl;
    std::cout << std::endl;
    CallNewArray<A>(2);  // call operator new[], operator delete[]
    std::cout << std::endl;
    CallNewArray<B>(2);  // call B::operator new[], B::operator delete[]
    std::cout << std::endl;

    std::cout << "------------------------------------------------------" << std::endl;
    std::cout << "> placement new" << std::endl;
    std::cout << std::endl;
    CallPlacementNew<A>();  // call placement new
    std::cout << std::endl;
    CallPlacementNew<B>();  // call B::placement new
    std::cout << std::endl;

    std::cout << "------------------------------------------------------" << std::endl;
    std::cout << "> placement new[]" << std::endl;
    std::cout << std::endl;
    CallPlacementNewArray<A>(2);  // call placement new[]
    std::cout << std::endl;
    CallPlacementNewArray<B>(2);  // call B::placement new[]
    std::cout << std::endl;

    std::cout << "------------------------------------------------------" << std::endl;
    std::cout << "> placement delete" << std::endl;
    std::cout << std::endl;
    CallPlacementNew<C>();  // call C::placement new, C::placement delete
    std::cout << std::endl;
    CallPlacementNewArray<C>(2);
    std::cout << std::endl;  // call C::placement new[], C::placement delete[]

    return 0;
}

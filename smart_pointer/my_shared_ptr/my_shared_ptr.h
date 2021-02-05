#pragma once

template <class T>
class MySharedPtr
{
    T** p;
    int* n;

    void reclaim()
    {
        *(this->n) -= 1;
        if (*(this->n) == 0) {
            delete *(this->p);
        }
    }

public:
    MySharedPtr(T* p)
    {
        this->p = new T*;
        this->n = new int;
        *(this->p) = p;
        *(this->n) = 1;
    }

    MySharedPtr(const MySharedPtr& ptr)
    {
        this->p = ptr.p;
        this->n = ptr.n;
        *(this->n) += 1;
    }

    MySharedPtr& operator=(const MySharedPtr& ptr)
    {
        if (*(this->p) == *(ptr.p)) {
            return *this;
        }

        reclaim();

        this->p = ptr.p;
        this->n = ptr.n;
        *(this->n) += 1;

        return *this;
    }


    ~MySharedPtr()
    {
        reclaim();
        delete this->p;
        delete this->n;
    }
};


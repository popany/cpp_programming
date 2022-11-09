#pragma once
#include <string>
#include <vector>
#include <memory>
#include <iostream>

class B {
public:
    virtual std::string name() = 0;
};

class Register {
public:
    static void add(B* p) {
        m().push_back(p);
    }

    static void print() {
        for (const auto& p : m()) {
            std::cout << p->name() << std::endl;
        }
    }

private:
    static std::vector<B*>& m() {
        static std::vector<B*> _;
        return _;
    }
};

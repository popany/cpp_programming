#include <iostream>

class B {
public:
    virtual std::string name() = 0;
};

class D1 : public B {
public:
    D1() {
        std::cout << "construct: " << name() << std::endl;
    }
    ~D1() {
        std::cout << "deconstruct: " << name() << std::endl;
    }

    std::string name() override  final {
        return "D1";
    }
};

static D1 d1;


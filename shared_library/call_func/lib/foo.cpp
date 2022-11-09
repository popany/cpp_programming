#include <iostream>
#include "../common.h"

class D1 : public B {
public:
    D1() {
        std::cout << "construct: " << name() << std::endl;
        Register::add(this);
    }
    ~D1() {
        std::cout << "deconstruct: " << name() << std::endl;
    }

    std::string name() override  final {
        return "D1";
    }
};

static D1 d1;

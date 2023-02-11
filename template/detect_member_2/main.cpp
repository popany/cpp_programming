// reference: https://gist.github.com/maddouri/0da889b331d910f35e05ba3b7b9d869b
#include <iostream>
#include <type_traits>
#include "has_member.h"

class A {
public:
    int x;
};

class B {
public:
    int x() { return 0; }
};

define_has_member(x);

int main() {
    std::cout << has_member(A, x) << std::endl;
    std::cout << has_member(B, x) << std::endl;

    return 0;
}


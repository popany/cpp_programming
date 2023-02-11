// reference: https://stackoverflow.com/a/16000226
#include <iostream>
#include <type_traits>

template <typename T, typename = int>
struct HasX : std::false_type { };

template <typename T>
struct HasX <T, decltype((void) T::x, 0)> : std::true_type {};

template <typename T, typename = int>
struct HasX2 : std::false_type { };

template <typename T>
struct HasX2 <T, decltype((void) &T::x, 0)> : std::true_type {};

class A {
public:
    int x;
};

class B {
public:
    int x() { return 0; }
};

int main() {
    std::cout << HasX<A>::value << std::endl;
    std::cout << HasX<B>::value << std::endl;

    std::cout << HasX2<A>::value << std::endl;
    std::cout << HasX2<B>::value << std::endl;

    return 0;
}


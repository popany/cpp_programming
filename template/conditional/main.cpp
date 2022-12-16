#include <iostream>
#include <type_traits>
#include <string>

template <bool use_string>
struct Foo {
    typedef typename std::conditional<use_string, std::string, int>::type Type;
    Type value;
};

int main() {
    Foo<false> a;
    Foo<true> b;

    a.value = 1;
    b.value = "1";

    return 0;
}

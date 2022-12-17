#include <iostream>
#include <type_traits>


template <bool print_yes>
std::enable_if_t<print_yes, void> print_func() {
    std::cout << "yes" << std::endl;
}

template <bool print_yes>
std::enable_if_t<!print_yes, void> print_func() {
    std::cout << "no" << std::endl;
}

template <bool print_yes>
struct Foo {
    void print() {
        print_func<print_yes>();
    }
};

int main() {
    Foo<true> yes;
    Foo<false> no;
    yes.print();
    no.print();

    return 0;
}

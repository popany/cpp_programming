// reference: https://stackoverflow.com/a/16000226
#include <iostream>
#include <type_traits>

template <typename T, typename = int>
struct HasFieldX : std::false_type { };

template <typename T>
struct HasFieldX <T, decltype((void) T::x, 0)> : std::true_type {};

template<typename, typename T>
struct HasFuncX;

template<typename C, typename Ret, typename... Args>
struct HasFuncX<C, Ret(Args...)> {
private:
    template<typename T>
    static constexpr auto check(T*)
    -> typename
        std::is_same<
            decltype( std::declval<T>().x( std::declval<Args>()... ) ),
            Ret    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        >::type;   // attempt to call it and see if the return type is correct

    template<typename>
    static constexpr std::false_type check(...);

    typedef decltype(check<C>(0)) type;

public:
    static constexpr bool value = type::value;
};

class A {
public:
    int x;
};

class B {
public:
    int x() { return 0; }
};

int main() {
    std::cout << HasFieldX<A>::value << std::endl;
    std::cout << HasFieldX<B>::value << std::endl;

    std::cout << HasFuncX<A, int()>::value << std::endl;
    std::cout << HasFuncX<B, int()>::value << std::endl;
    return 0;
}


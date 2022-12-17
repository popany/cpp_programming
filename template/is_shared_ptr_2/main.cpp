#include <iostream>
#include <memory>
#include <type_traits>
#include <utility>

template<typename T> struct is_shared_ptr : std::false_type {};
template<typename T> struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};

template <typename T, std::enable_if_t<is_shared_ptr<std::remove_cv_t<std::remove_reference_t<T>>>::value, bool> _ = true>
decltype(auto) get(T&& t) {
    std::cout << "shared_ptr" << std::endl;
    return *(t.get());
}

template <typename T, std::enable_if_t<!is_shared_ptr<std::remove_cv_t<std::remove_reference_t<T>>>::value, bool> _ = true>
decltype(auto) get(T&& t) {
    std::cout << "no shared_ptr" << std::endl;
    return t;
}


int main() {
    const int a = 0;
    const std::shared_ptr<int> b = std::make_shared<int>(1);

    const int& x = get(a);
    const int& y = get(b);
    static_assert(std::is_same<const int&, decltype(get(a))>::value, "");
    static_assert(std::is_same<int&, decltype(get(b))>::value, "");

    std::cout << (uint64_t)(void*)&a << " == " << (uint64_t)(void*)&x << std::endl;
    std::cout << (uint64_t)(void*)b.get() << " == " << (uint64_t)(void*)&y << std::endl;

    return 0;
}


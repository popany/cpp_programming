// https://www.appsloveworld.com/cplus/100/78/how-to-detect-if-a-type-is-shared-ptr-at-compile-time

#include <iostream>
#include <memory>
#include <type_traits>
#include <utility>

template<typename T> struct is_shared_ptr : std::false_type {};
template<typename T> struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};

template <class T> 
typename std::enable_if<is_shared_ptr<decltype(std::declval<T>().value)>::value, void>::type
func( T t )
{
    std::cout << "shared ptr" << std::endl;
}

template <class T> 
typename std::enable_if<!is_shared_ptr<decltype(std::declval<T>().value)>::value, void>::type
func( T t )
{
    std::cout << "non shared" << std::endl;
}

struct Foo {
    std::shared_ptr<int> value;
};

struct Bar {
    int value;
};

int main() {
    Foo a;
    Bar b;

	func(a);
	func(b);

	return 0;
}


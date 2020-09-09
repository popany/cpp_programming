#include <iostream>
#include <thread>
#include <stdexcept>

void JoinNotJoinableThread1()
{
    try {
        std::thread t([](){
            std::cout << "run 1" << std::endl;
        });
        std::thread t2(std::move(t));
        t2.join();
        t.join();
    } catch (const std::system_error &e) {
        std::cout << "[" << __FUNCTION__ << "] exception: " << e.what() << std::endl;
    }
}

void JoinNotJoinableThread2()
{
    try {
        std::thread t;
        t.join();
    } catch (const std::system_error &e) {
        std::cout << "[" << __FUNCTION__ << "] exception: " << e.what() << std::endl;
    }
}

void CallThreadJoin1(std::thread& t)
{
    try {
        t.join();
    } catch (const std::system_error &e) {
        std::cout << "[" << __FUNCTION__ << "] exception: " << e.what() << std::endl;
    }
}

void JoinSelf1()
{
    try {
        std::thread t;
        t = std::thread([&](){
            CallThreadJoin1(t);
        });
        t.join();
    } catch (const std::system_error &e) {
        std::cout << "[" << __FUNCTION__ << "] exception: " << e.what() << std::endl;
    }
}

void CallThreadJoin2(std::thread& t)
{
    try {
        if (t.joinable()) {
            t.join();
        } else {
            std::cout << "[" << __FUNCTION__ << "] not joinable" << std::endl;
        }
    } catch (const std::system_error &e) {
        std::cout << "[" << __FUNCTION__ << "] exception: " << e.what() << std::endl;
    }
}

void JoinSelf2()
{
    try {
        std::thread t;
        t = std::thread([&](){
            CallThreadJoin2(t);
        });
        t.join();
    } catch (const std::system_error &e) {
        std::cout << "[" << __FUNCTION__ << "] exception: " << e.what() << std::endl;
    }
}

int main()
{
    JoinNotJoinableThread1(); // exception: Invalid argument
    JoinNotJoinableThread2(); // exception: Invalid argument
    JoinSelf1(); // exception: Resource deadlock avoided
    JoinSelf2(); // exception: Resource deadlock avoided

    return 0;
}

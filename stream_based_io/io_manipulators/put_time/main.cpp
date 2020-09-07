#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <thread>
#include <chrono>
#include <mutex>
#include <ctime>

std::string GetTimeStamp()
{
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) -
              std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch());

    std::tm tm;
    auto t = std::chrono::system_clock::to_time_t(now);
    tm = *std::localtime(&t); // not thread-safe

    std::stringstream ss;
    ss << std::put_time(&tm, "%Y%m%d-%H%M%S.") << ms.count();
    return ss.str();
}

int main()
{
    std::cout << "timestamp: " << GetTimeStamp() << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "timestamp: " << GetTimeStamp() << std::endl;

    return 0;
}

#include <iostream>
#include <string>
#include <chrono>
#include <thread>

class Stopwatch
{
    std::chrono::time_point<std::chrono::steady_clock> startTime;
    std::chrono::nanoseconds elapsed;

public:
    Stopwatch()
    {
        Start();
    }

    void Start()
    {
        startTime = std::chrono::steady_clock::now();
    }

    void Stop()
    {
        elapsed = std::chrono::steady_clock::now() - startTime;
    }

    double ElapsedMs()
    {
        return std::chrono::duration<double, std::milli>(elapsed).count();
    }
};

int main()
{
    Stopwatch sp;
    sp.Start();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    sp.Stop();
    std::cout << "time elasped: " << sp.ElapsedMs() << "ms" << std::endl;

    return 0;
}

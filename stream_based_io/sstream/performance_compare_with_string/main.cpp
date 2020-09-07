#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>

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

size_t UseStringStream(std::string s, int n, int repeat)
{
    size_t r = 0;
    for (int i = 0; i < repeat; i++) {
        std::stringstream ss;
        for (int j = 0; j < n; j++) {
            ss << s;
        }
        r += ss.str().length();
    }
    return r;
}

size_t UseString(std::string s, int n, int repeat)
{
    size_t r = 0;
    for (int i = 0; i < repeat; i++) {
        std::string ss;
        for (int j = 0; j < n; j++) {
            ss += s;
        }
        r += ss.length();
    }
    return r;
}
 
int main()
{
    Stopwatch stopwatch;

    std::string originString = "abcdefghijklmnopqrstuvwxyz";
    int copyCount = 2;
    int repeatTimes = 1000000;
    
    stopwatch.Start();
    size_t totalLen = UseStringStream(originString, copyCount, repeatTimes);
    stopwatch.Stop();

    std::cout << "use stringstream consumes: " << stopwatch.ElapsedMs() << "ms, total length: " << totalLen << std::endl;


    stopwatch.Start();
    totalLen = UseString(originString, copyCount, repeatTimes);
    stopwatch.Stop();

    std::cout << "use string consumes: " << stopwatch.ElapsedMs() << "ms, total length: " << totalLen << std::endl;

    return 0;
}


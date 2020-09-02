#pragma once
#include <string>
#include <functional>

class RequestHandler
{
    std::function<void(std::string)> write;

public:
    RequestHandler(std::function<void(std::string)> write):
        write(write)
    {}

    void virtual Append(std::string s) = 0;
    bool virtual CheckIntegrity() = 0;
    void virtual ReadCompleteCallback(int errorCode) = 0;
    void virtual WriteCompleteCallback(int errorCode) = 0;
    void virtual Write(std::string s)
    {
        write(s);
    }
};

#pragma once
#include <string>
#include <functional>

class RequestHandler
{
    std::function<void(std::string)> write;
    
protected:
    bool isReadWaiting;
    bool isWriteWaiting;

    void virtual Write(std::string s)
    {
        write(s);
    }

public:
    RequestHandler(std::function<void(std::string)> write):
        write(write),
        isReadWaiting(true),
        isWriteWaiting(false)
    {}

    void virtual AppendReceivedData(std::string s) = 0;
    bool virtual CheckDataIntegrity() = 0;
    void virtual ReadCompleteCallback(int errorCode) = 0;
    void virtual WriteCompleteCallback(int errorCode) = 0;
    bool IsWaiting()
    {
        return isReadWaiting || isWriteWaiting;
    }
};

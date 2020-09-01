#pragma once
#include <string>

class RequestHandler
{
public:
    void virtual Append(std::string s) = 0;
    bool virtual CheckIntegrity() = 0;
    void virtual ReadCompleteCallback(int errorCode) = 0;
    void virtual WriteCompleteCallback(int errorCode) = 0;
    void virtual Write(std::string s) = 0;
};

#pragma once
#include <string>
#include <functional>

class SocketHandler
{
    int fd;
    bool isEof;
    std::function<void(int)> close;
    
public:
    SocketHandler(int fd, std::function<void(int)> close);
    void Close();
    bool IsEof();
    std::string Read();
    std::string Write(std::string s);
};

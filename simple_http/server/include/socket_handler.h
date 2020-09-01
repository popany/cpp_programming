#pragma once
#include <string>
#include <functional>

class SocketHandler
{
    int fd;
    std::function<void(int)> close;
    
public:
    SocketHandler(int fd, std::function<void(int)> close);
    void Close();
    std::string Read();
    std::string Write(std::string s);
};

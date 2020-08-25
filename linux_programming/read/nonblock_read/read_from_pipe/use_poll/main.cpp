#include <sys/poll.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define MAX_EVENTS 10

inline std::string GetErrorMsg(std::string info, int errorCode)
{
    return info + ", error code(" + std::to_string(errorCode) + "), " + strerror(errorCode);
}

FILE* Popen(const char* command, const char* type)
{
    FILE* f = popen(command, type);
    if (f == NULL) {
        int code = errno;
        throw std::runtime_error(GetErrorMsg("popen failed", code));
    }
    return f;
}

void GetInput(int fd)
{
    const size_t bufSize = 3;
    std::vector<char> buf(bufSize, 0);
    ssize_t n = read(fd, &buf[0], bufSize); // will not block
    if (n == 0) {
        std::cout << "eof<" << fd << ">" << std::endl;
    } else if (n < 0) {
        int code = errno;
        std::cout << GetErrorMsg(std::string("read failed") + std::to_string(fd), code) << std::endl;
    } else {
        std::cout << "input<" << fd << ">: " << &buf[0] << std::endl;
    }
}

void CreateFile(const std::string filePath)
{
    if (creat(&filePath[0], O_CREAT) == -1) {
        int code = errno;
        std::cout << GetErrorMsg(std::string("creat failed, filePath: \"") + filePath + "\"" , code) << std::endl;
    }
}

std::string GetTailCommand(const std::string filePath)
{
    return std::string("tail -f ") + filePath;
}

void Run()
{
	std::string filePath1 = "/tmp/1.txt";
    std::string filePath2 = "/tmp/2.txt";

    CreateFile(filePath1);
    CreateFile(filePath2);

    FILE* f1 = Popen(GetTailCommand(filePath1).c_str(), "r");
    FILE* f2 = Popen(GetTailCommand(filePath2).c_str(), "r");
    int fd1 = fileno(f1);
    int fd2 = fileno(f2);
	
    std::vector<pollfd> pollFds(2);
    pollFds[0].fd = fd1;
    pollFds[0].events = POLLIN;
	pollFds[1].fd = fd2;
    pollFds[1].events = POLLIN;

    for (;;) {
        int timeoutMs = 1000;
        int rv = poll(&pollFds[0], 2, timeoutMs);
        if (rv > 0) {
            if (pollFds[0].revents) {
                GetInput(fd1);
            }
            if (pollFds[1].revents) {
                GetInput(fd2);
            }
        } else if (rv < 0) {
            int code = errno;
            std::cout << GetErrorMsg("poll failed", code) << std::endl;
            break;
        } else {
            std::cout << "timeout" << std::endl;
        }
    }
}

int main()
{
    try {
        Run();
    } catch (const std::exception& e) {
        std::cout << "exception: " << e.what() << std::endl;
    }

    return 0;
}


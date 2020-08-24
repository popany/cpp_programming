#include <sys/poll.h>
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

void Run()
{
    int fd = fileno(stdin);
    pollfd pollFd;
    pollFd.fd = fd;
    pollFd.events = POLLIN;

    for (;;) {
        int timeoutMs = 1000;
        int rv = poll(&pollFd, 1, timeoutMs);
        if (rv > 0) {
            const size_t bufSize = 3;
            std::vector<char> buf(bufSize, 0);
            ssize_t n = read(fd, &buf[0], bufSize); // will not block
            if (n == 0) {
                std::cout << "eof" << std::endl;
                break;
            } else if (n < 0) {
                int code = errno;
                std::cout << GetErrorMsg("read failed", code) << std::endl;
            } else {
                std::cout << "input: " << &buf[0] << std::endl;
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


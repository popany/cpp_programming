#include <sys/select.h>
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
    fd_set fdSet;

    for (;;) {
        FD_ZERO(&fdSet);
        FD_SET(fd, &fdSet);
        
        timeval timeout = { 1, 0 };
        int rv = select(fd+1, &fdSet, NULL, NULL, &timeout);
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
            std::cout << GetErrorMsg("select failed", code);
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


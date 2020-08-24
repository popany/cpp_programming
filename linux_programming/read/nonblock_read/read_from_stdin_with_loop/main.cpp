#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <string.h>

inline std::string GetErrorMsg(std::string info, int errorCode)
{
    return info + ", error code(" + std::to_string(errorCode) + "), " + strerror(errorCode);
}

static void SetNonBlocking(int fd)
{
    int flags  = fcntl(fd, F_GETFL, 0 );
    flags |= O_NONBLOCK;
    flags = fcntl(fd, F_SETFL, flags);
}


void Run()
{
    int fd = fileno(stdin);
    const size_t bufSize = 3;

    SetNonBlocking(fd);

    for (;;) {
        std::vector<char> buf(bufSize + 1, 0);
        ssize_t rv = read(fd, &buf[0], bufSize);
        if (rv == EAGAIN) {
            int code = errno;
            std::cout << GetErrorMsg("no data", code) << std::endl;
        } else if (rv < 0) {
            int code = errno;
            if (code == EAGAIN) {
                std::cout << GetErrorMsg("no data", code) << std::endl;
            } else {
                std::cout << GetErrorMsg("read failed", code) << std::endl;
            }
        } else if (rv > 0) {
            std::cout << "input: " << &buf[0] << std::endl;
        } else {
            std::cout << "eof" << std::endl;
            break;
        }
        sleep(1);
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


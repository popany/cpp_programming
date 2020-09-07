#include <unistd.h>
#include <stdint.h>
#include <stdexcept>
#include <string>
#include <vector>
#include "utils.h"
#include "log.h"
#include "socket_handler.h"

SocketHandler::SocketHandler(int fd, std::function<void(int)> close):
    fd(fd),
    close(close)
{}

void SocketHandler::Close()
{
    close(fd);
}

std::string SocketHandler::Read()
{
    std::string s{};
    for (;;) {
        const size_t bufSize = 1000;
        std::vector<char> buf(bufSize, 0);

        ssize_t n = read(fd, &buf[0], bufSize);
        if (n < 0) {
            int code = errno;
            if (code == EAGAIN) {
                break;
            }
            throw std::runtime_error(GetErrorMsg(std::string(__PRETTY_FUNCTION__) + ", read", code));
        } else if (n > 0) {
            s += &buf[0];
        } else {
            break;
        }
    }

    return s;
}

std::string SocketHandler::Write(std::string s)
{
    if (s.empty()) {
        return std::string{};
    }
    size_t totalWriteLen = 0;
    for (;;) {
        if (s.empty()) {
            break;
        }
        ssize_t n = write(fd, s.data(), s.size());
        if (n < 0) {
            int code = errno;
            if (code == EAGAIN) {
                break;
            }
            throw std::runtime_error(GetErrorMsg(std::string(__PRETTY_FUNCTION__) + ", write failed", code));
        }
        else {
            totalWriteLen += n;
            s.erase(0, n);
        }
    }

    if (totalWriteLen == 0) {
        LogDebug("invalid write");
    }

    return s;
}

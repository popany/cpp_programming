#include <sys/poll.h>
#include <arpa/inet.h>
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

std::string Read(int fd)
{
    const size_t bufSize = 512;
    std::vector<char> buf(bufSize + 1, 0);
    ssize_t n = read(fd, &buf[0], bufSize); // will not block
    if (n < 0) {
        int code = errno;
        throw std::runtime_error(GetErrorMsg("Read() failed", code));
    }
    return std::string(&buf[0]);
}

std::string Write(int fd, const std::string s)
{
    ssize_t n = write(fd, s.c_str(), s.size()); // will not block
    if (n < 0) {
        int code = errno;
        throw std::runtime_error(GetErrorMsg("Read() failed", code));
    }
    return s.substr(n, s.size() - n);
}

int Connect(std::string ip, uint16_t port)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        int code = errno;
        throw std::runtime_error(GetErrorMsg("Connect() failed, socket", code));
    }

    sockaddr_in addr;
    explicit_bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) != 1) {
        throw std::runtime_error("Connect() failed, inet_pton");
    }

    if (connect(fd, (sockaddr*)&addr, sizeof(addr)) == -1) {
        int code = errno;
        throw std::runtime_error(GetErrorMsg("Connect() failed, connect", code));
    }
    return fd;
}

void Run()
{
    const std::string ip = "127.0.0.1";
    const uint16_t port = 5666;

    pollfd pollFds[2];
    pollFds[0].fd = fileno(stdin);
    pollFds[0].events = POLLIN;
    pollFds[1].fd = Connect(ip, port);
    pollFds[1].events = POLLIN;

    std::string r{};

    for (;;) {
        int timeoutMs = 3000;
        int n = poll(pollFds, 2, timeoutMs);
        if (n > 0) {
            if (pollFds[0].revents) {
                r += Read(pollFds[0].fd);
                if (r.size() > 0) {
                    pollFds[1].events |= POLLOUT;
                }
            }
            if (pollFds[1].revents & POLLIN) {
                std::string s = Read(pollFds[1].fd);
                if (s.size() == 0) {
                    std::cout << "peer closed" << std::endl;
                    break;
                }
                std::cout << ">" << s << std::endl;
            }
            if (pollFds[1].revents & POLLOUT) {
                std::string tmp = r;
                r = Write(pollFds[1].fd, r);
                if (r.size() == 0) {
                    pollFds[1].events &= ~POLLOUT;
                }
                std::cout << "<" << tmp.substr(0, tmp.size() - r.size()) << std::endl;
            }
        } else if (n < 0) {
            int code = errno;
            throw std::runtime_error(GetErrorMsg("poll failed", code));
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


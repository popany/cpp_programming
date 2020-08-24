#include <sys/epoll.h>
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

template <int events_count>
class Epoll
{
    int epollFd;
    std::vector<epoll_event> events;   

    void CreateEpollFd()
    {
        epollFd = epoll_create1(0);
        if (epollFd == -1) {
            int code = errno;
            throw std::runtime_error(GetErrorMsg("epoll_create1 failed", code));
        }
    }

public:

    Epoll(): events(events_count)
    {}

    void Init()
    {
        CreateEpollFd();
    }

    void Add(int fd, uint32_t eventMask)
    {
        epoll_event event;
        event.data.fd = fd;
        event.events = eventMask;
        if (epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &event) == -1) {
            int code = errno;
            throw std::runtime_error(GetErrorMsg("epoll_ctl failed", code));
        }
    }

    int Wait(int timeoutMs)
    {
        for (;;) {
            int n = epoll_wait(epollFd, &events[0], events_count, timeoutMs);
            if (n == -1) {
                int code = errno;
                if (code == EINTR) {
                    std::cout << "timeout" << std::endl;
                    continue;
                }
                throw std::runtime_error(GetErrorMsg("epoll_wait failed", code));
            }
            return n;
        }
    }
	
	const epoll_event* GetEvent(int i)
	{
		return &events[i];
	}
	
	int GetFd(int i)
	{
		return GetEvent(i)->data.fd;
	}
};

void GetInput(int fd)
{
    const size_t bufSize = 3;
    std::vector<char> buf(bufSize, 0);
    ssize_t n = read(fd, &buf[0], bufSize); // will not block
    if (n == 0) {
        std::cout << "eof<" << fd << ">" << std::endl;
    } else if (n < 0) {
        int code = errno;
        throw std::runtime_error(GetErrorMsg(std::string("read failed<") + std::to_string(fd) + ">", code));
    } else {
        std::cout << "input<" << fd << ">: " << &buf[0] << std::endl;
    }
}

void CloseFile(int fd)
{
    if (close(fd) == -1) {
        int code = errno;
        std::cout << GetErrorMsg(std::string("close failed") + std::to_string(fd), code) << std::endl;
    }
}

void Run()
{
    Epoll<MAX_EVENTS> ep;
    ep.Init();
    int fd = fileno(stdin);
    ep.Add(fd, EPOLLIN);
    CloseFile(fd);
    
    for (;;) {   
        int rv = ep.Wait(1000);
        if (rv > 0) {
			for (int i = 0; i < rv; i++) {
				GetInput(ep.GetFd(i)); // Bad file descriptor, because fd is closed
			}
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


#include <sys/epoll.h>
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
	
    Epoll<MAX_EVENTS> ep;
    ep.Init();
    ep.Add(fd1, EPOLLIN);
    ep.Add(fd2, EPOLLIN);
    
    for (;;) {   
        int rv = ep.Wait(1000);
        if (rv > 0) {
			for (int i = 0; i < rv; i++) {
				GetInput(ep.GetFd(i));
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


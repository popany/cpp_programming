#pragma once
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <functional>
#include <memory>
#include "utils.h"
#include "log.h"
#include "socket_handler.h"
#include "common_types.h"

void SetExitCondition();

class Epoll;
class SocketMap;

class Server
{
    const uint16_t port;
    int listenFd;
    uint64_t nextConnId;

    std::shared_ptr<Epoll> ep;
    std::shared_ptr<SocketMap> socketMap;

    bool CheckSocketExist(int fd);
    void Listen();
    int Accept(std::string& ip, uint16_t& port);
    void CloseFd(int fd);
    void ProcessNewConnection();
    void ProcessEvent(int fd, uint32_t eventMask);
    void ProcessEvents(int n);
    void Release();

    std::function<void(uint64_t, ClientInfo, SocketHandler)> reportNewConnection;
    std::function<void(uint64_t)> reportCanRead;
    std::function<void(uint64_t)> reportCanWrite;
    std::function<void(uint64_t)> reportErrorOccurred;
    std::function<void(uint64_t)> reportTimeToExit;

public:
    Server(uint16_t port, size_t epEventCount);
    void Start();
    void WaitForExit();
    void RegisterReportFunctions(
        std::function<void(uint64_t, ClientInfo, SocketHandler)> newConnection,
        std::function<void(uint64_t)> canRead,
        std::function<void(uint64_t)> canWrite,
        std::function<void(uint64_t)> errorOccurred,
        std::function<void(uint64_t)> timeToExit
    );
};

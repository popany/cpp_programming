#include <signal.h>
#include <stdexcept>
#include "server.h"

static bool isTimeToExit = false;

static void SigHandler(int sigNum)
{
    if (sigNum == SIGINT) {
        isTimeToExit = true;
    }
}

void SetExitCondition()
{
    if (signal(SIGINT, SigHandler) == SIG_ERR) {
        int code = errno;
        throw std::runtime_error(GetErrorMsg(std::string(__PRETTY_FUNCTION__) + ", signal failed", code));
    }
}

class Epoll
{
    int epollFd;
    std::vector<epoll_event> events;   

    void CreateEpollFd()
    {
        epollFd = epoll_create1(0);
        if (epollFd == -1) {
            int code = errno;
            throw std::runtime_error(GetErrorMsg(std::string(__PRETTY_FUNCTION__) + ", epoll_create1 failed", code));
        }
    }

public:

    Epoll(size_t eventCount): events(eventCount)
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
            throw std::runtime_error(GetErrorMsg(std::string(__PRETTY_FUNCTION__) + ", epoll_ctl failed", code));
        }
    }

    void Delete(int fd)
    {
        LogDebug("delete fd: ", fd);
        if (epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, NULL) == -1) {
            int code = errno;
            throw std::runtime_error(GetErrorMsg(std::string(__PRETTY_FUNCTION__) + ", epoll_ctl failed", code));
        }
    }

    int Wait(int timeoutMs)
    {
        int n = epoll_wait(epollFd, events.data(), events.size(), timeoutMs);
        if (n == -1) {
            int code = errno;
            if (code == EINTR) {
                return 0;
            }
            throw std::runtime_error(GetErrorMsg(std::string(__PRETTY_FUNCTION__) + ", epoll_wait failed", code));
        }
        return n;
    }

    int GetFd(int i) const
    {
        return events[i].data.fd;
    }

    uint32_t GetEvent(int i) const
    {
        return events[i].events;
    }
};

class SocketMap
{
    std::map<int, uint64_t> sockets;
    std::mutex m;

public:
    void Add(int fd, uint64_t connectionId)
    {
        std::lock_guard<std::mutex> lk(m);
        sockets[fd] = connectionId;
    }

    void Delete(int fd)
    {
        std::lock_guard<std::mutex> lk(m);
        if (sockets.count(fd) == 0) {
            throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + ", fd(" + std::to_string(fd) + ") not exist");
        }
        sockets.erase(fd);
    }

    uint64_t GetValue(int fd)
    {
        std::lock_guard<std::mutex> lk(m);
        return sockets[fd];
    }

    size_t Count(int fd)
    {
        std::lock_guard<std::mutex> lk(m);
        return sockets.count(fd);
    }

    std::vector<uint64_t> GetValues()
    {
        std::lock_guard<std::mutex> lk(m);
        std::vector<uint64_t> values;
        for (auto e : sockets) {
            values.push_back(e.second);
        }
        return values;
    }
};

bool Server::CheckSocketExist(int fd)
{
    return socketMap->Count(fd);
}

void Server::Listen()
{
    listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenFd == -1) {
        int code = errno;
        throw std::runtime_error(GetErrorMsg(std::string(__PRETTY_FUNCTION__) + ", socket failed", code));
    }
    sockaddr_in addr;
    explicit_bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    if (bind(listenFd, (sockaddr*)&addr, sizeof(addr)) == -1) {
        int code = errno;
        throw std::runtime_error(GetErrorMsg(std::string(__PRETTY_FUNCTION__) + ", bind failed", code));
    }

    const int LISTEN_BACKLOG = 50;
    if (listen(listenFd, LISTEN_BACKLOG) == -1) {
        int code = errno;
        throw std::runtime_error(GetErrorMsg(std::string(__PRETTY_FUNCTION__) + ", listen failed", code));
    }
}

int Server::Accept(std::string& ip, uint16_t& port)
{
    sockaddr_in addr;
    socklen_t len = sizeof(addr);
    int fd = accept(listenFd, (sockaddr*)&addr, &len);
    if (fd == -1) {
        int code = errno;
        throw std::runtime_error(GetErrorMsg(std::string(__PRETTY_FUNCTION__) + ", accept failed", code));
    }

    std::vector<char> buf(INET_ADDRSTRLEN + 1, 0);
    if (inet_ntop(AF_INET, &addr.sin_addr, &buf[0], INET_ADDRSTRLEN) == NULL) {
        int code = errno;
        throw std::runtime_error(GetErrorMsg(std::string(__PRETTY_FUNCTION__) + ", inet_ntop failed", code));
    }
    ip = &buf[0];
    port = ntohs(addr.sin_port);
    return fd;
}

void Server::CloseFd(int fd)
{
    LogDebug("close fd: ", fd);
    if (close(fd) == -1) {
        int code = errno;
        throw std::runtime_error(GetErrorMsg(std::string(__PRETTY_FUNCTION__) + "close fd(" + std::to_string(fd), code) + ") failed");
    }
}

static void SetNonBlocking(int fd)
{
    int flags  = fcntl(fd, F_GETFL, -1 );
    flags |= O_NONBLOCK;
    flags = fcntl(fd, F_SETFL, flags);
}

void Server::ProcessNewConnection()
{
    std::string ip{};
    uint16_t port;
    int fd = Accept(ip, port);
    if (CheckSocketExist(fd)) {
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + "fd(" + std::to_string(fd) +") exist");
    }

    SetNonBlocking(fd);
    ep->Add(fd, EPOLLIN | EPOLLOUT | EPOLLET);

    nextConnId++;
    socketMap->Add(fd, nextConnId);

    LogDebug("connection id=", nextConnId, ", ", "fd=", fd);
    reportNewConnection(nextConnId, ClientInfo{ip, port},
        SocketHandler(
            fd, 
            [&](int fdv){
                ep->Delete(fdv);
                CloseFd(fdv);
                socketMap->Delete(fdv);
            })
    );
}

void Server::ProcessEvent(int fd, uint32_t eventMask)
{
    if (!CheckSocketExist(fd)) {
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + ", fd(" + std::to_string(fd) +") not exist");
    }

    if (eventMask & EPOLLIN) {
        reportCanRead(socketMap->GetValue(fd));
    }
    if (eventMask & EPOLLOUT) {
        reportCanWrite(socketMap->GetValue(fd));
    }
    if (eventMask & EPOLLERR) {
        reportErrorOccurred(socketMap->GetValue(fd));
    }
}

void Server::ProcessEvents(int n)
{
    for (int i = 0; i < n; i++) {
        if (ep->GetFd(i) == listenFd) {
            ProcessNewConnection();
        }
        else {
            ProcessEvent(ep->GetFd(i), ep->GetEvent(i));
        }
    }
}

void Server::Release()
{
    std::vector<uint64_t> connectionIds = socketMap->GetValues();
    for (auto connectionId : connectionIds) {
        reportTimeToExit(connectionId);
    }

    ep->Delete(listenFd);
    CloseFd(listenFd);
}

void Server::Start()
{
    LogInfo("server start, port: ", port);
    Listen();
    ep->Init();
    ep->Add(listenFd, EPOLLIN);

    for (;;) {   
        const int timeoutMs = 3000;
        int n = ep->Wait(timeoutMs);
        if (n > 0) {
            ProcessEvents(n);
        } else {
            LogDebug("epoll wait timeout(", timeoutMs, "ms)");
        }
        if (isTimeToExit) {
            Release();
            break;
        }
    }
}

void Server::WaitForExit()
{
    ;
}

void Server::RegisterReportFunctions(
    std::function<void(uint64_t, ClientInfo, SocketHandler)> newConnection,
    std::function<void(uint64_t)> canRead,
    std::function<void(uint64_t)> canWrite,
    std::function<void(uint64_t)> errorOccurred,
    std::function<void(uint64_t)> timeToExit)
{
    this->reportNewConnection = newConnection;
    this->reportCanRead = canRead;
    this->reportCanWrite = canWrite;
    this->reportErrorOccurred = errorOccurred;
    this->reportTimeToExit = timeToExit;
}

Server::Server(uint16_t port, size_t epEventCount):
    nextConnId(0),
    port(port),
    ep(std::make_shared<Epoll>(epEventCount)),
    socketMap(std::make_shared<SocketMap>())
{}

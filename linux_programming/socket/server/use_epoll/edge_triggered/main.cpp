#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <map>
#include <functional>
#include <memory>

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
            throw std::runtime_error(GetErrorMsg("Epoll::CreateEpollFd() failed, epoll_create1", code));
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
            throw std::runtime_error(GetErrorMsg("Epoll.Add() failed", code));
        }
    }

    void DeleteFd(int fd)
    {
        std::cout << "Epoll::DeleteFd(), fd=" << fd << std::endl;
        if (epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, NULL) == -1) {
            int code = errno;
            throw std::runtime_error(GetErrorMsg("Epoll.DeleteFd() failed", code));
        }
    }

    int Wait(int timeoutMs)
    {
        int n = epoll_wait(epollFd, &events[0], events_count, timeoutMs);
        if (n == -1) {
            int code = errno;
            if (code == EINTR) {
                return 0;
            }
            throw std::runtime_error(GetErrorMsg("Epoll.Wait() failed", code));
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

void SetNonBlocking(int fd)
{
    int flags  = fcntl(fd, F_GETFL, 0 );
    flags |= O_NONBLOCK;
    flags = fcntl(fd, F_SETFL, flags);
}

class ConnHandler
{
    int fd;
    std::string ip;
    uint16_t port;
    bool isEof;
    std::function<void(int)> closeConn;
    
public:

    ConnHandler(int fd, std::string ip, uint16_t port, std::function<void(int)> closeConn):
        fd(fd),
        ip(ip),
        port(port),
        isEof(false),
        closeConn(closeConn)
    {}

    void CloseConn()
    {
        closeConn(fd);
    }

    std::string GetPeerInfo()
    {
        return ip + ":" + std::to_string(port) + ", fd=" + std::to_string(fd);
    }

    bool IsEof()
    {
        return isEof;
    }

    std::string Read()
    {
        std::string s{};
        for (;;) {
            const size_t bufSize = 3;
            std::vector<char> buf(bufSize, 0);

            ssize_t n = read(fd, &buf[0], bufSize);
            if (n < 0) {
                int code = errno;
                if (code == EAGAIN) {
                    break;
                }
                throw std::runtime_error(GetErrorMsg("ConnHandler::Read(), read", code));
            } else if (n > 0) {
                s += &buf[0];
            } else {
                isEof = true;
                break;
            }
        }
        return s;
    }

    std::string Write(std::string s)
    {
        if (s.empty()) {
            return std::string{};
        }
        for (;;) {
            if (s.empty()) {
                break;
            }
            ssize_t n = write(fd, s.c_str(), s.size());
            if (n < 0) {
                int code = errno;
                if (code == EAGAIN) {
                    break;
                }
                throw std::runtime_error(GetErrorMsg("Write() failed", code));
            }
            else {
                s = s.substr(n, s.size() - n);
            }
        }
        return s;
    }
};

class Server
{
    const uint16_t port;
    int listenFd;
    uint64_t nextConnId;
    std::map<int, uint64_t> conns;

    Epoll<MAX_EVENTS> ep;

    void Listen()
    {
        listenFd = socket(AF_INET, SOCK_STREAM, 0);
        if (listenFd == -1) {
            int code = errno;
            throw std::runtime_error(GetErrorMsg("Server::Listen() failed, socket", code));
        }
        sockaddr_in addr;
        explicit_bzero(&addr, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(port);

        if (bind(listenFd, (sockaddr*)&addr, sizeof(addr)) == -1) {
            int code = errno;
            throw std::runtime_error(GetErrorMsg("Server::Listen() failed, bind", code));
        }

        const int LISTEN_BACKLOG = 50;
        if (listen(listenFd, LISTEN_BACKLOG) == -1) {
            int code = errno;
            throw std::runtime_error(GetErrorMsg("Server::Listen() failed, listen", code));
        }
    }

    int Accept(std::string& ip, uint16_t& port)
    {
        sockaddr_in addr;
        socklen_t len = sizeof(addr);
        int fd = accept(listenFd, (sockaddr*)&addr, &len);
        if (fd == -1) {
            int code = errno;
            throw std::runtime_error(GetErrorMsg("Server::Accept() failed", code));
        }

        std::vector<char> buf(INET_ADDRSTRLEN + 1, 0);
        if (inet_ntop(AF_INET, &addr.sin_addr, &buf[0], INET_ADDRSTRLEN) == NULL) {
            int code = errno;
            throw std::runtime_error(GetErrorMsg("Server::Accept() failed, inet_ntop", code));
        }
        ip = &buf[0];
        port = ntohs(addr.sin_port);
        return fd;
    }

    void CloseFd(int fd)
    {
        std::cout << "Server::CloseFd(), fd=" << fd << std::endl;
        if (close(fd) == -1) {
            int code = errno;
            throw std::runtime_error(GetErrorMsg("Server::CloseFd() failed", code));
        }
    }

    void ProcessNewConnection()
    {
        std::string ip{};
        uint16_t port;
        int fd = Accept(ip, port);
        if (conns.count(fd) != 0) {
            throw std::runtime_error("Server::ProcessNewConnection(), fd exist");
        }

        nextConnId++;
        SetNonBlocking(fd);
        ep.Add(fd, EPOLLIN | EPOLLOUT | EPOLLET);

        NewConnection(nextConnId,
            ConnHandler(
                fd, 
                ip,
                port,
                [&](int fdv){
                    ep.DeleteFd(fdv);
                    CloseFd(fdv);
                    conns.erase(fdv);
                })
        );
        conns[fd] = nextConnId;
    }

    void ProcessEvent(int fd, uint32_t eventMask)
    {
        if (conns.count(fd) == 0) {
            throw std::runtime_error("Server::ProcessEvent(), fd not exist");
        }

        if (eventMask & EPOLLIN) {
            CanRead(conns[fd]);
        }
        if (eventMask & EPOLLOUT) {
            CanWrite(conns[fd]);
        }
        if (eventMask & EPOLLERR) {
            ErrorOccurred(conns[fd]);
        }
    }

    void ProcessEvents(int n)
    {
        for (int i = 0; i < n; i++) {
            if (ep.GetFd(i) == listenFd) {
                ProcessNewConnection();
            }
            else {
                ProcessEvent(ep.GetFd(i), ep.GetEvent(i));
            }
        }
    }

public:
    std::function<void(uint64_t, ConnHandler)> NewConnection;
    std::function<void(uint64_t)> CanRead;
    std::function<void(uint64_t)> CanWrite;
    std::function<void(uint64_t)> ErrorOccurred;

    Server(uint16_t port): nextConnId(0), port(port)
    {}

    void Start()
    {
        Listen();
        ep.Init();
        ep.Add(listenFd, EPOLLIN);
	
        for (;;) {   
            const int timeoutMs = 3000;
            int n = ep.Wait(timeoutMs);
            if (n > 0) {
                ProcessEvents(n);
            } else {
                std::cout << "epoll wait timeout(" + std::to_string(timeoutMs) + "ms)" << std::endl;
            }
        }
    }
};

class Session
{
    uint64_t id;
    std::string input;
    std::string output;
    std::string peerInfo;

    bool isReadable;
    bool isWritable;
    bool isClosed;

    ConnHandler connHandler;

    std::function<void()> releaseSession;

    bool PeerClosed()
    {
        return connHandler.IsEof();
    }

public:
    Session(uint64_t id, ConnHandler connHandler, std::function<void()> releaseSession):
        id(id),
        connHandler(connHandler),
        releaseSession(releaseSession),
        isReadable(false),
        isWritable(false),
        isClosed(false)
    {
        peerInfo = connHandler.GetPeerInfo();
    }

    const std::string& GetPeerInfo()
    {
        return peerInfo;
    }

    void SetReadable()
    {
        isReadable = true;
    }

    void SetWritable()
    {
        isWritable = true;
    }

    void Read()
    {
        if (!isReadable) {
            return;
        }

        std::string s = connHandler.Read();
        std::cout << "Session::Read(), " << GetPeerInfo() << ". \"" << s << "\"" << std::endl;
        s.erase(std::remove(s.begin(), s.end(), '\n'), s.end());
        input += s;
        for (;;) {
            size_t pos = input.find('.');
            if (pos != std::string::npos) {
                std::string tmp = input.substr(0, pos);
                input = input.substr(pos + 1, input.size() - pos - 1);
                output += "[" + std::string(tmp.rbegin(), tmp.rend()) + "]";
            } else {
                break;
            }
        }

        isReadable = false;
        Write();
    }

    void Write()
    {
        if (!isWritable) {
            return;
        }

        output = connHandler.Write(output);

        isWritable = false;
        
        if (PeerClosed() && output.empty()) {
            Close();
        }
    }

    void Close()
    {
        if (isClosed) {
            throw std::runtime_error("Session::Close(), session is closed");
        }
        connHandler.CloseConn();
        releaseSession();
        std::cout << "Session::Close(), " << GetPeerInfo() << std::endl;
        isClosed = true;
    }
};

class SessionManager
{
    std::map<uint64_t, std::shared_ptr<Session>> sessions;

    void ReleaseSession(uint64_t id)
    {
        VerifySessionExist(id);
        sessions.erase(id);
    }
   
public:
    SessionManager()
    {}

    void VerifySessionExist(uint64_t id)
    {
        if (sessions.count(id) == 0) {
            throw std::runtime_error("SessionManager::VerifySessionExist()");
        }
    }

    void VerifySessionNotExist(uint64_t id)
    {
        if (sessions.count(id) != 0) {
            throw std::runtime_error("SessionManager::VerifySessionNotExist()");
        }
    }


    void NewConnection(uint64_t id, ConnHandler connHandler)
    {
        VerifySessionNotExist(id);
        std::shared_ptr<Session> session = std::make_shared<Session>(id, connHandler, std::bind(&SessionManager::ReleaseSession, this, id));
        sessions[id] = session;
        std::cout << "SessionManager::NewConnection(), " << session->GetPeerInfo() << ", session count: " << sessions.size() << std::endl;
    }

    void CanRead(uint64_t id)
    {
        VerifySessionExist(id);
        auto session = sessions[id];
        std::cout << "SessionManager::CanRead(), " << session->GetPeerInfo() << std::endl;

        session->SetReadable();
        session->Read();
    }

    void CanWrite(uint64_t id)
    {
        VerifySessionExist(id);
        auto session = sessions[id];
        std::cout << "SessionManager::CanWrite(), " << session->GetPeerInfo() << std::endl;

        session->SetWritable();
        session->Write();
    }

    void ErrorOccurred(uint64_t id)
    {
        VerifySessionExist(id);
        auto session = sessions[id];
        std::cout << "SessionManager::ErrorOccurred(), " << session->GetPeerInfo() << std::endl;

        session->Close();
    }
};

void RegisterServerCallback(Server& server, SessionManager& sessionMgr)
{
    server.NewConnection = std::bind(&SessionManager::NewConnection, &sessionMgr, std::placeholders::_1, std::placeholders::_2);
    server.CanRead = std::bind(&SessionManager::CanRead, &sessionMgr, std::placeholders::_1);
    server.CanWrite = std::bind(&SessionManager::CanWrite, &sessionMgr, std::placeholders::_1);
    server.ErrorOccurred = std::bind(&SessionManager::ErrorOccurred, &sessionMgr, std::placeholders::_1);
}

int main()
{
    const uint16_t port = 5666;
    try {
        Server server(port);
        SessionManager sessionMgr;
        RegisterServerCallback(server, sessionMgr);

        server.Start();
    } catch (const std::exception& e) {
        std::cout << "exception: " << e.what() << std::endl;
    }

    return 0;
}


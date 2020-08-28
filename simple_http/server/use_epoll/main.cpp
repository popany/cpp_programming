#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <map>
#include <functional>
#include <memory>

#define MAX_EVENTS 10

static bool timeToExit = false;

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

    void Delete(int fd)
    {
        std::cout << "Epoll::Delete(), fd=" << fd << std::endl;
        if (epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, NULL) == -1) {
            int code = errno;
            throw std::runtime_error(GetErrorMsg("Epoll.Delete() failed", code));
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
            const size_t bufSize = 1000;
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
            ssize_t n = write(fd, s.data(), s.size());
            if (n < 0) {
                int code = errno;
                if (code == EAGAIN) {
                    break;
                }
                throw std::runtime_error(GetErrorMsg("Write() failed", code));
            }
            else {
                s.erase(0, n);
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
            throw std::runtime_error(GetErrorMsg("Server::CloseFd() failed, fd=" + std::to_string(fd), code));
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
                    ep.Delete(fdv);
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

    void Release()
    {
        while (!conns.empty()) {
            TimeToExit(conns.begin()->second);
        }

        ep.Delete(listenFd);
        CloseFd(listenFd);
    }

public:
    std::function<void(uint64_t, ConnHandler)> NewConnection;
    std::function<void(uint64_t)> CanRead;
    std::function<void(uint64_t)> CanWrite;
    std::function<void(uint64_t)> ErrorOccurred;
    std::function<void(uint64_t)> TimeToExit;

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
            if (timeToExit) {
                Release();
                break;
            }
        }
    }
};

static inline void Ltrim(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) { return !std::isspace(ch); }));
}

static inline void Rtrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) { return !std::isspace(ch); }).base(), s.end());
}

static inline void Trim(std::string &s)
{
    Ltrim(s);
    Rtrim(s);
}

class HttpRequest
{
    constexpr static char LINE_END[] = "\r\n";
    constexpr static size_t LINE_END_LEN = sizeof(LINE_END) - 1;
    constexpr static char HEADER_SEP = ':';
    constexpr static size_t HEADER_SEP_LEN = 1;
    constexpr static char CONTENT_LENGTH_HEADER[] = "Content-Length";

    std::string buf;

    std::string firstLine;
    std::map<std::string, std::string> headers;
    std::string messageBody;
    size_t contentLength;

    bool headersParseComplete;
    bool totalParseComplete;

    void SetFirstLine()
    {
        if (!firstLine.empty()) {
            return;
        }

        size_t pos = buf.find(LINE_END);
        if (pos == std::string::npos) {
            return;
        }

        firstLine = buf.substr(0, pos);
        buf.erase(0, pos + LINE_END_LEN);
    }

    void AddHeader(const std::string& s)
    {
        size_t pos = s.find(HEADER_SEP);
        if (pos == std::string::npos) {
            throw std::runtime_error(__FUNCTION__);
        }
        std::string v = s.substr(pos + HEADER_SEP_LEN);
        Trim(v);
        headers[s.substr(0, pos)] = v;
    }

    void SetContentLength()
    {
        if (headers.count(CONTENT_LENGTH_HEADER) == 0) {
            contentLength = 0;
            return;
        }
        contentLength = std::stoi(headers[CONTENT_LENGTH_HEADER]);
    }

    void SetHeaders()
    {
        for (;;) {
            size_t pos = buf.find(LINE_END);
            if (pos == std::string::npos) {
                return;
            }

            if (pos == 0) {
                buf.erase(0, LINE_END_LEN);
                headersParseComplete = true;
                SetContentLength();
                return;
            }

            AddHeader(buf.substr(0, pos));
            buf.erase(0, pos + LINE_END_LEN);
        }
    }

    void ParseMessageBody()
    {
        if (!headersParseComplete) {
            return;
        }
        if (totalParseComplete) {
            return;
        }

        messageBody += buf;
        if (messageBody.size() >= contentLength) {
            totalParseComplete = true;
        }

        buf.clear();
    }

    void ParseRequest()
    {
        if (!headersParseComplete) {
            SetFirstLine();
            SetHeaders();
        }
        ParseMessageBody();
    }

public:
    HttpRequest():
        headersParseComplete(false),
        totalParseComplete(false),
        contentLength(0)
    {}

    void Append(const std::string& s)
    {
        if (totalParseComplete) {
            return;
        }
        buf += s;
        ParseRequest();
    }

    void Reset()
    {
        buf.clear();
    }

    void Print()
    {
        std::cout << "first line: " << firstLine << std::endl;
        for (const auto x : headers) {
            std::cout << x.first << ": " << x.second << std::endl;
        }
        std::cout << "message body: " << messageBody << std::endl; 
    }

    bool AllReceived()
    {
        return totalParseComplete;
    }

    std::string GetMessageBody()
    {
        return messageBody;
    }
};
constexpr char HttpRequest::LINE_END[];
constexpr size_t HttpRequest::LINE_END_LEN;
constexpr char HttpRequest::HEADER_SEP;
constexpr size_t HttpRequest::HEADER_SEP_LEN;
constexpr char HttpRequest::CONTENT_LENGTH_HEADER[];

class Session
{
    uint64_t id;
    std::string peerInfo;

    HttpRequest request;
    std::string response;

    bool isReadable;
    bool isWritable;
    bool isClosed;
    bool processComplete;

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
        isClosed(false),
        processComplete(false)
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

    void Process()
    {
        request.Print();
        std::string receivedMsg = request.GetMessageBody();
        std::string responseMsg = "hello\r\n";
        
        response = "HTTP/1.1 200 OK\r\n";
        response += "Context-Length: " +  std::to_string(responseMsg.size()) + "\r\n";
        response += "\r\n";
        response += responseMsg;

        processComplete = true;
    }

    bool CanClose()
    {
        if (!response.empty()) {
            return false;
        }

        if (PeerClosed()) {
            return true;
        }

        return processComplete;
    }

    void Read()
    {
        if (!isReadable) {
            return;
        }

        std::string s = connHandler.Read();
        std::cout << "Session::Read(), " << GetPeerInfo() << ". read: \"" << s << "\"" << std::endl;

        request.Append(s);

        if (request.AllReceived()) {
            Process();
        }

        isReadable = false;
        Write();
    }

    void Write()
    {
        if (!isWritable) {
            return;
        }

        response = connHandler.Write(response);

        isWritable = false;
        
        if (CanClose()) {
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

    void TimeToExit(uint64_t id)
    {
        VerifySessionExist(id);
        auto session = sessions[id];
        std::cout << "SessionManager::TimeToExit(), " << session->GetPeerInfo() << std::endl;

        session->Close();
    }
};

void RegisterServerCallback(Server& server, SessionManager& sessionMgr)
{
    server.NewConnection = std::bind(&SessionManager::NewConnection, &sessionMgr, std::placeholders::_1, std::placeholders::_2);
    server.CanRead = std::bind(&SessionManager::CanRead, &sessionMgr, std::placeholders::_1);
    server.CanWrite = std::bind(&SessionManager::CanWrite, &sessionMgr, std::placeholders::_1);
    server.ErrorOccurred = std::bind(&SessionManager::ErrorOccurred, &sessionMgr, std::placeholders::_1);
    server.TimeToExit = std::bind(&SessionManager::TimeToExit, &sessionMgr, std::placeholders::_1);
}

void SigHandler(int sigNum)
{
    if (sigNum == SIGINT) {
        timeToExit = true;
    }
}

void SetExitCondition()
{
    if (signal(SIGINT, SigHandler) == SIG_ERR) {
        int code = errno;
        throw std::runtime_error(GetErrorMsg("SetExitCondition(), signal", code));
    }
}

int main()
{
    const uint16_t port = 5666;

    try {
        SetExitCondition();

        Server server(port);
        SessionManager sessionMgr;
        RegisterServerCallback(server, sessionMgr);

        server.Start();
    } catch (const std::exception& e) {
        std::cout << "exception: " << e.what() << std::endl;
    }

    return 0;
}


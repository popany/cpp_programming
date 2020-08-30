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
#include <fstream>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <map>
#include <functional>
#include <memory>
#include <mutex>

#define MAX_EVENTS 10

static bool timeToExit = false;

inline std::string GetErrorMsg(std::string info, int errorCode)
{
    return info + ", error code(" + std::to_string(errorCode) + "), " + strerror(errorCode);
}

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

class Config
{
    enum {
        DEFAULT_PORT = 5666,
        EP_EVENT_COUNT = 10,
    };

    void ParseLine(std::string s)
    {
        Trim(s);
        size_t pos = s.find('=');
        if (pos == std::string::npos) {
            return;
        }

        std::string name = s.substr(0, pos);
        Trim(name);
        std::string value = s.substr(pos + 1);
        Trim(value);
        
        if (name == "port") {
            port = std::stoi(value);
        } else if (name == "log_level") {
            logLevel = value;
        }
    }

    Config():
        port(DEFAULT_PORT),
        epEventCount(EP_EVENT_COUNT)
    {
        std::ifstream fs("./config");
        if (fs.fail()) {
            std::cout << "failed to open config file" << std::endl;
            exit(1);
        }
        
        while (!fs.eof()) {
            std::string s;
            std::getline(fs, s);
            ParseLine(s);
        }
    }

public:

    std::string logLevel;
    uint16_t port;
    size_t epEventCount;

    Config(const Config&) = delete;
    void operator=(const Config&) = delete;

    static Config& GetInstance()
    {
        static Config config;
        return config;
    }
};

class Logger
{
    enum {
        LEVEL_ERROR = 0,
        LEVEL_WARN,
        LEVEL_INFO,
        LEVEL_DEBUG,
        LEVEL_ALWAYS
    };

    int logLevel;
    std::ofstream fs;
    std::mutex m;

    Logger():
        logLevel(LEVEL_ALWAYS)
    {
        fs.open("./log.txt");
        if (!fs.is_open()) {
            std::cout << "failed to open logfile" << std::endl;
            exit(1);
        }
    }

    template <typename T>
    void Log(T t) 
    {
        fs << t << "\n";
        std::cout << t << "\n";
    }

    template<typename T, typename... Args>
    void Log(T t, Args... args)
    {
        fs << t;
        std::cout << t;
        Log(args...) ;
    }

public:
    Logger(const Logger&) = delete;
    void operator=(const Logger&) = delete;

    static Logger& GetInstance()
    {
        static Logger instance;
        return instance;
    }

    ~Logger()
    {
        fs.close();
    }

    void SetLogLevel(std::string s)
    {
        if (s == "error") {
            logLevel = LEVEL_ERROR;
            LogAlways("SetLogLevel: error");
        } else if (s == "warn") {
            logLevel = LEVEL_WARN;
            LogAlways("SetLogLevel: warn");
        } else if (s == "info") {
            logLevel = LEVEL_INFO;
            LogAlways("SetLogLevel: info");
        } else if (s == "debug") {
            logLevel = LEVEL_DEBUG;
            LogAlways("SetLogLevel: debug");
        }
    }

    template<typename... Args>
    void LogAlways(Args... args)
    {
        std::lock_guard<std::mutex> lk(m);
        Log(args...);
    }

    template<typename... Args>
    void LogInfo(Args... args)
    {
        if (logLevel < LEVEL_INFO) {
            return;
        }

        std::lock_guard<std::mutex> lk(m);
        Log("[Info]", args...);
    }

    template<typename... Args>
    void LogWarn(Args... args)
    {
        if (logLevel < LEVEL_WARN) {
            return;
        }

        std::lock_guard<std::mutex> lk(m);
        Log("[Warn]", args...);
    }

    template<typename... Args>
    void LogError(Args... args)
    {
        if (logLevel < LEVEL_ERROR) {
            return;
        }

        std::lock_guard<std::mutex> lk(m);
        Log("[Error]", args...);
    }

    template<typename... Args>
    void LogDebug(Args... args)
    {
        if (logLevel < LEVEL_DEBUG) {
            return;
        }

        std::lock_guard<std::mutex> lk(m);
        Log("[Debug]", args...);
    }
};

#define LogInfo(...) Logger::GetInstance().LogInfo(std::string("[")+__PRETTY_FUNCTION__+"] ",__VA_ARGS__)
#define LogDebug(...) Logger::GetInstance().LogDebug(std::string("[")+__PRETTY_FUNCTION__+"] ",__VA_ARGS__)
#define LogWarn(...) Logger::GetInstance().LogWarn(std::string("[")+__PRETTY_FUNCTION__+"] ",__VA_ARGS__)
#define LogError(...) Logger::GetInstance().LogError(std::string("[")+__PRETTY_FUNCTION__+"] ",__VA_ARGS__)
#define LogAlways(...) Logger::GetInstance().LogAlways(std::string("[")+__PRETTY_FUNCTION__+"] ",__VA_ARGS__)

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

void SetNonBlocking(int fd)
{
    int flags  = fcntl(fd, F_GETFL, 0 );
    flags |= O_NONBLOCK;
    flags = fcntl(fd, F_SETFL, flags);
}

class SocketHandler
{
    int fd;
    bool isEof;
    std::function<void(int)> close;
    
public:

    SocketHandler(int fd, std::function<void(int)> close):
        fd(fd),
        isEof(false),
        close(close)
    {}

    void Close()
    {
        close(fd);
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
                throw std::runtime_error(GetErrorMsg(std::string(__PRETTY_FUNCTION__) + ", read", code));
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
                throw std::runtime_error(GetErrorMsg(std::string(__PRETTY_FUNCTION__) + ", write failed", code));
            }
            else {
                s.erase(0, n);
            }
        }
        return s;
    }
};

struct ClientInfo
{
    std::string ip;
    uint16_t port;
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

    uint64_t operator[](int fd)
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

class Server
{
    const uint16_t port;
    int listenFd;
    uint64_t nextConnId;

    Epoll ep;
    SocketMap socketMap;

    bool CheckSocketExist(int fd)
    {
        return socketMap.Count(fd);
    }

    void Listen()
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

    int Accept(std::string& ip, uint16_t& port)
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

    void CloseFd(int fd)
    {
        LogDebug("close fd: ", fd);
        if (close(fd) == -1) {
            int code = errno;
            throw std::runtime_error(GetErrorMsg(std::string(__PRETTY_FUNCTION__) + "close fd(" + std::to_string(fd), code) + ") failed");
        }
    }

    void ProcessNewConnection()
    {
        std::string ip{};
        uint16_t port;
        int fd = Accept(ip, port);
        if (CheckSocketExist(fd)) {
            throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + "fd(" + std::to_string(fd) +") exist");
        }

        SetNonBlocking(fd);
        ep.Add(fd, EPOLLIN | EPOLLOUT | EPOLLET);

        nextConnId++;
        socketMap.Add(fd, nextConnId);

        LogDebug("connection id=", nextConnId, ", ", "fd=", fd);
        NewConnection(nextConnId, ClientInfo{ip, port},
            SocketHandler(
                fd, 
                [&](int fdv){
                    ep.Delete(fdv);
                    CloseFd(fdv);
                    socketMap.Delete(fdv);
                })
        );
    }

    void ProcessEvent(int fd, uint32_t eventMask)
    {
        if (!CheckSocketExist(fd)) {
            throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + ", fd(" + std::to_string(fd) +") not exist");
        }

        if (eventMask & EPOLLIN) {
            CanRead(socketMap[fd]);
        }
        if (eventMask & EPOLLOUT) {
            CanWrite(socketMap[fd]);
        }
        if (eventMask & EPOLLERR) {
            ErrorOccurred(socketMap[fd]);
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
        std::vector<uint64_t> connectionIds = socketMap.GetValues();
        for (auto connectionId : connectionIds) {
            TimeToExit(connectionId);
        }

        ep.Delete(listenFd);
        CloseFd(listenFd);
    }

public:
    std::function<void(uint64_t, ClientInfo, SocketHandler)> NewConnection;
    std::function<void(uint64_t)> CanRead;
    std::function<void(uint64_t)> CanWrite;
    std::function<void(uint64_t)> ErrorOccurred;
    std::function<void(uint64_t)> TimeToExit;

    Server(uint16_t port): nextConnId(0), port(port), ep(Config::GetInstance().epEventCount)
    {}

    void Start()
    {
        LogInfo("server start, port: ", port);
        Listen();
        ep.Init();
        ep.Add(listenFd, EPOLLIN);

        for (;;) {   
            const int timeoutMs = 3000;
            int n = ep.Wait(timeoutMs);
            if (n > 0) {
                ProcessEvents(n);
            } else {
                LogDebug("epoll wait timeout(", timeoutMs, "ms)");
            }
            if (timeToExit) {
                Release();
                break;
            }
        }
    }
};

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
            throw std::runtime_error(__PRETTY_FUNCTION__);
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

    void PrintRequest()
    {
        LogDebug("first line: ", firstLine);
        for (const auto x : headers) {
            LogDebug(x.first, ": ", x.second);
        }
        LogDebug("message body: ", messageBody);
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

class Connection
{
    uint64_t id;
    ClientInfo clientInfo;

    HttpRequest request;
    std::string response;

    bool isReadable;
    bool isWritable;
    bool isClosed;
    bool processComplete;

    SocketHandler socketHandler;

    std::function<void()> release;

    bool ClientClosed()
    {
        return socketHandler.IsEof();
    }

public:
    Connection(uint64_t id, ClientInfo clientInfo, SocketHandler socketHandler, std::function<void()> release):
        id(id),
        clientInfo(clientInfo),
        socketHandler(socketHandler),
        release(release),
        isReadable(false),
        isWritable(false),
        isClosed(false),
        processComplete(false)
    {
    }

    std::string GetClientInfo()
    {
        return "<connection id: " + std::to_string(id) + ">(" + clientInfo.ip + ":" + std::to_string(clientInfo.port) + ")";
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
        request.PrintRequest();
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

        if (ClientClosed()) {
            return true;
        }

        return processComplete;
    }

    void Read()
    {
        if (!isReadable) {
            return;
        }

        std::string s = socketHandler.Read();
        LogDebug(GetClientInfo(), ". read: \"", s, "\"");

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

        response = socketHandler.Write(response);

        isWritable = false;
        
        if (CanClose()) {
            Close();
        }
    }

    void Close()
    {
        if (isClosed) {
            throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + ", Connection is closed");
        }
        socketHandler.Close();
        release();
        LogDebug(GetClientInfo());
        isClosed = true;
    }
};

class ConnectionMap
{
    std::map<uint64_t, std::shared_ptr<Connection>> connections;
    std::mutex m;

public:
    std::shared_ptr<Connection> operator[](uint64_t id)
    {
        std::lock_guard<std::mutex> lk(m);
        return connections[id];
    }

    void Add(uint64_t id, std::shared_ptr<Connection> connection)
    {
        std::lock_guard<std::mutex> lk(m);
        connections[id] = connection;
    }

    void Erase(uint64_t id)
    {
        std::lock_guard<std::mutex> lk(m);
        connections.erase(id);
    }

    size_t Count(uint64_t id)
    {
        std::lock_guard<std::mutex> lk(m);
        return connections.count(id);
    }

    size_t Size()
    {
        std::lock_guard<std::mutex> lk(m);
        return connections.size();
    }
};

class ConnectionManager
{
    ConnectionMap connectionMap;

    void ReleaseConnection(uint64_t id)
    {
        VerifyConnectionExist(id);
        connectionMap.Erase(id);
    }

    void VerifyConnectionExist(uint64_t id)
    {
        if (connectionMap.Count(id) == 0) {
            throw std::runtime_error(std::string(__PRETTY_FUNCTION__));
        }
    }

    void VerifyConnectionNotExist(uint64_t id)
    {
        if (connectionMap.Count(id) != 0) {
            throw std::runtime_error(std::string(__PRETTY_FUNCTION__));
        }
    }

public:
    ConnectionManager()
    {}

    void NewConnection(uint64_t id, ClientInfo clientInfo, SocketHandler socketHandler)
    {
        VerifyConnectionNotExist(id);
        std::shared_ptr<Connection> connection = std::make_shared<Connection>(id, clientInfo, socketHandler, std::bind(&ConnectionManager::ReleaseConnection, this, id));

        connectionMap.Add(id, connection);
        LogDebug(connection->GetClientInfo(), ", connection count: ", connectionMap.Size());
    }

    void CanRead(uint64_t id)
    {
        VerifyConnectionExist(id);
        auto connection = connectionMap[id];
        LogDebug(connection->GetClientInfo());

        connection->SetReadable();
        connection->Read();
    }

    void CanWrite(uint64_t id)
    {
        VerifyConnectionExist(id);
        auto connection = connectionMap[id];
        LogDebug(connection->GetClientInfo());

        connection->SetWritable();
        connection->Write();
    }

    void ErrorOccurred(uint64_t id)
    {
        VerifyConnectionExist(id);
        auto connection = connectionMap[id];
        LogDebug(connection->GetClientInfo());

        connection->Close();
    }

    void TimeToExit(uint64_t id)
    {
        VerifyConnectionExist(id);
        auto connection = connectionMap[id];
        LogDebug(connection->GetClientInfo());

        connection->Close();
    }
};

void RegisterServerCallback(Server& server, ConnectionManager& connectionMgr)
{
    server.NewConnection = std::bind(&ConnectionManager::NewConnection, &connectionMgr, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    server.CanRead = std::bind(&ConnectionManager::CanRead, &connectionMgr, std::placeholders::_1);
    server.CanWrite = std::bind(&ConnectionManager::CanWrite, &connectionMgr, std::placeholders::_1);
    server.ErrorOccurred = std::bind(&ConnectionManager::ErrorOccurred, &connectionMgr, std::placeholders::_1);
    server.TimeToExit = std::bind(&ConnectionManager::TimeToExit, &connectionMgr, std::placeholders::_1);
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
        throw std::runtime_error(GetErrorMsg(std::string(__PRETTY_FUNCTION__) + ", signal failed", code));
    }
}

int main()
{
    Logger::GetInstance().SetLogLevel(Config::GetInstance().logLevel);

    try {
        SetExitCondition();

        Server server(Config::GetInstance().port);
        ConnectionManager connectionMgr;
        RegisterServerCallback(server, connectionMgr);

        server.Start();
    } catch (const std::exception& e) {
        LogError("exception: ", e.what());
    }

    return 0;
}


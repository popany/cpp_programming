#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <error.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <string>
#include <thread>

#ifdef USE_POLL
#  include <poll.h>
#endif

// https://man7.org/linux/man-pages/man7/ip.7.html
// https://man7.org/linux/man-pages/man2/bind.2.html
// https://man7.org/linux/man-pages/man2/listen.2.html

#define ERROR_MESSAGE(code) (std::string(__FUNCTION__) + ", " + strerror(code))
#define LISTEN_BACKLOG 50

class Config
{
public:
    std::string dstAddr;
    int dstPort;
    std::string srcAddr;
    int srcPort;
    bool listen;
    bool accept;
    bool connect;
    bool bind;
    bool read;
    bool write;

    Config():
        srcAddr("0.0.0.0"),
        srcPort(10000),
        dstAddr("127.0.0.1"),
        dstPort(10001),
        connect(false),
        bind(false),
        listen(false),
        accept(false),
        read(false),
        write(false)
    {}

    void set(int argc, const char* argv[])
    {
        for (int i = 1; i < argc; ) {
            if (*(uint16_t*)argv[i] == *(uint16_t*)"-h") {
                std::cout << "-l                        Listen\n";
                std::cout << "-a                        Accept\n";
                std::cout << "-b <srcAddr> <srcPort>    Bind\n";
                std::cout << "-c <dstAddr> <dstPort>    Connect\n";
                std::cout << "-r                        Read\n";
                std::cout << "-w                        Write\n";
                std::cout << std::endl;
                exit(0);
            } else if (*(uint16_t*)argv[i] == *(uint16_t*)"-r") {
                read = true;
                i++;
            } else if (*(uint16_t*)argv[i] == *(uint16_t*)"-w") {
                write = true;
                i++;
            } else if (*(uint16_t*)argv[i] == *(uint16_t*)"-l") {
                listen = true;
                i++;
            } else if (*(uint16_t*)argv[i] == *(uint16_t*)"-a") {
                accept = true;
                i++;
            } else if (*(uint16_t*)argv[i] == *(uint16_t*)"-b") {
                if (i + 3 > argc) {
                    throw std::runtime_error("parameter error");
                }
                srcAddr = std::string(argv[i + 1]);
                srcPort = std::stoi(argv[i + 2]);
                bind = true;
                i += 3;
            } else if (*(uint16_t*)argv[i] == *(uint16_t*)"-c") {
                if (i + 3 > argc) {
                    throw std::runtime_error("parameter error");
                }
                dstAddr = std::string(argv[i + 1]);
                dstPort = std::stoi(argv[i + 2]);
                connect = true;
                i += 3;
            } else {
                throw std::runtime_error("parameter error");
            }
        }

        if (accept && connect) {
            throw std::runtime_error("parameter conflict");
        }
    }
};

void Signal(int signum, sighandler_t handler)
{
    if (signal(signum, handler) == SIG_ERR) {
        int errorCode = errno;
        throw std::runtime_error(ERROR_MESSAGE(errorCode));
    }
}

void SetNonBlocking(int fd)
{
    int flags  = fcntl(fd, F_GETFL, -1 );
    flags |= O_NONBLOCK;
    flags = fcntl(fd, F_SETFL, flags);
}

void SetBlocking(int fd)
{
    int flags  = fcntl(fd, F_GETFL, -1 );
    flags &= ~O_NONBLOCK;
    flags = fcntl(fd, F_SETFL, flags);
}

int Socket(int domain, int type, int protocol)
{
    int fd = socket(domain, type, protocol);
    if (fd == -1) {
        int errorCode = errno;
        throw std::runtime_error(ERROR_MESSAGE(errorCode));
    }
    return fd;
}

void Bind(int fd, const std::string& addr, int port)
{
    sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;

    if (inet_aton(addr.c_str(), &(servaddr.sin_addr)) == 0) {
        int errorCode = errno;
        throw std::runtime_error(ERROR_MESSAGE(errorCode));
    }
    servaddr.sin_port = htons(port);

    if (bind(fd, (sockaddr *) &servaddr, sizeof(servaddr)) == -1) {
        int errorCode = errno;
        throw std::runtime_error(ERROR_MESSAGE(errorCode));
    }
}

void Listen(int fd, int backlog = LISTEN_BACKLOG)
{
    if (listen(fd, backlog) == 1) {
        int errorCode = errno;
        throw std::runtime_error(ERROR_MESSAGE(errorCode));
    }
}

void Connect(int fd, const std::string& addr, int port)
{
    sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;

    if (inet_aton(addr.c_str(), &(servaddr.sin_addr)) == 0) {
        int errorCode = errno;
        throw std::runtime_error(ERROR_MESSAGE(errorCode));
    }
    servaddr.sin_port = htons(port);

    if (connect(fd, (sockaddr *) &servaddr, sizeof(servaddr)) == -1) {
        int errorCode = errno;
        throw std::runtime_error(ERROR_MESSAGE(errorCode));
    }
}

int Accept(int fd)
{
    int connFd = accept(fd, NULL, NULL);
    if (connFd == -1) {
        int errorCode = errno;
        throw std::runtime_error(ERROR_MESSAGE(errorCode));
    }
    return connFd;
}

std::string Read(int fd, bool& peerClosed)
{
    peerClosed = false;
    std::string s{};
    const size_t bufSize = 1000;
    std::vector<char> buf(bufSize + 1, 0);

    for (;;) {
        ssize_t n = read(fd, &buf[0], bufSize);
        if (n < 0) {
            int errorCode = errno;
            if (errorCode == EAGAIN) {
                break;
            }
            throw std::runtime_error(ERROR_MESSAGE(errorCode));
        } else if (n > 0) {
            buf[n] = 0;
            s += &buf[0];
        } else {
            peerClosed = true;
            break;
        }
    }
    return s;
}

std::string Read(int fd)
{
    bool peerClosed = false;
    return Read(fd, peerClosed);
}

std::string Write(int fd, std::string s)
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
            int errorCode = errno;
            if (errorCode == EAGAIN) {
                break;
            }
            throw std::runtime_error(ERROR_MESSAGE(errorCode));
        } else {
            totalWriteLen += n;
            s.erase(0, n);
        }
    }

    return s;
}

void Shutdown(int sockfd, int how)
{
    if (shutdown(sockfd, how) == -1) {
        int errorCode = errno;
        throw std::runtime_error(ERROR_MESSAGE(errorCode));
    }
}

#ifdef USE_POLL

int Poll(struct pollfd *fds, nfds_t nfds, int timeout)
{
    int nReady = poll(fds, nfds, timeout);

    if (nReady == -1) {
        int errorCode = errno;
        throw std::runtime_error(ERROR_MESSAGE(errorCode));
    }
    return nReady;
}

void Process(int fd, const Config& config)
{
    SetNonBlocking(fd);
    SetNonBlocking(STDIN_FILENO);

    std::vector<pollfd> fds(3);

    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;
    fds[1].fd = fd;
    fds[1].events = POLLIN | POLLOUT;

    std::string w{};

    for (;;) {
        Poll(&fds[0], fds.size(), -1);

        if (fds[1].revents & POLLIN && config.read) {
            bool fdClosed = false;
            std::string s = Read(fd, fdClosed);
            if (!s.empty()) {
                std::cout << s;
                std::cout.flush();
            }
            if (fdClosed) {
                fds[1].events &= ~POLLIN;
            }
        }

        if (fds[0].revents & POLLIN) {
            std::string r = Read(STDIN_FILENO);
            if (r == "exit\n") {
                SetBlocking(STDIN_FILENO);
                break;
            } else if (r == "shutdown read\n") {
                std::cout << "SHUT_RD" << std::endl;
                Shutdown(fd, SHUT_RD);
                continue;
            } else if (r == "shutdown write\n") {
                std::cout << "SHUT_WR" << std::endl;
                Shutdown(fd, SHUT_WR);
                continue;
            } else if (r == "shutdown both\n") {
                std::cout << "SHUT_RDWR" << std::endl;
                Shutdown(fd, SHUT_RDWR);
                continue;
            }

            if (config.write) {
                w += r;
            }
        }

        if (fds[1].revents & POLLOUT || !w.empty()) {
            w = Write(fd, w);
        }

        if (!w.empty()) {
            fds[1].events |= POLLOUT;
        } else {
            fds[1].events &= ~POLLOUT;
        }
    }
}

# else

int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
{
    int nReady = select(nfds, readfds, writefds, exceptfds, timeout);

    if (nReady == -1) {
        int errorCode = errno;
        throw std::runtime_error(ERROR_MESSAGE(errorCode));
    }
    return nReady;
}

void Process(int fd, const Config& config)
{
    SetNonBlocking(fd);
    SetNonBlocking(STDIN_FILENO);

    fd_set rSet;
    FD_ZERO(&rSet);
    FD_SET(STDIN_FILENO, &rSet);
    FD_SET(fd, &rSet);

    fd_set wSet;
    FD_ZERO(&wSet);

    bool fdClosed = false;
    std::string w{};

    for (;;) {
        Select(fd + 1, &rSet, &wSet, NULL, NULL);

        if (FD_ISSET(fd, &rSet) && config.read) {
            std::string s = Read(fd, fdClosed);
            if (!s.empty()) {
                std::cout << s;
                std::cout.flush();
            }
            if (fdClosed) {
                FD_CLR(fd, &rSet);
            }
        }
        if (!fdClosed) {
            FD_SET(fd, &rSet);
        }

        if (FD_ISSET(STDIN_FILENO, &rSet)) {
            std::string r = Read(STDIN_FILENO);
            if (r == "exit\n") {
                SetBlocking(STDIN_FILENO);
                break;
            } else if (r == "shutdown read\n") {
                std::cout << "SHUT_RD" << std::endl;
                Shutdown(fd, SHUT_RD);
                continue;
            } else if (r == "shutdown write\n") {
                std::cout << "SHUT_WR" << std::endl;
                Shutdown(fd, SHUT_WR);
                continue;
            } else if (r == "shutdown both\n") {
                std::cout << "SHUT_RDWR" << std::endl;
                Shutdown(fd, SHUT_RDWR);
                continue;
            }

            if (config.write) {
                w += r;
            }
        } else {
            FD_SET(STDIN_FILENO, &rSet);
        }

        if (FD_ISSET(fd, &wSet) || !w.empty()) {
            w = Write(fd, w);
        }

        if (!w.empty()) {
            FD_SET(fd, &wSet);
        }
    }
}

#endif

void TypeEToExit()
{
    for (;;) {
        std::cout << "type 'e' to exit" << std::endl;
        std::string s;
        std::cin >> s;
        if (s == "e") {
            break;
        }
    }
}

void Run(const Config& config)
{
    int fd = Socket(AF_INET, SOCK_STREAM, 0);

    if (config.bind) {
        Bind(fd, config.srcAddr, config.srcPort);
    }

    if (config.listen) {
        Listen(fd);
    }

    if (config.accept) {
        int connFd = Accept(fd);
        Process(connFd, config);
    }

    if (config.connect) {
        Connect(fd, config.dstAddr, config.dstPort);    
        Process(fd, config);
    }

    TypeEToExit();
}

int main(int argc, const char* argv[])
{
    try {
        Config config;
        config.set(argc, argv);

        Signal(SIGPIPE, SIG_IGN);
        Run(config);
    } catch (const std::exception& ex) {
        std::cout << ex.what() << std::endl;
    }

    return 0;
}


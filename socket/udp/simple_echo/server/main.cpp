#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <iostream>
#include <string>

#define ERROR_MESSAGE(code) (std::string(__FUNCTION__) + ", " + strerror(code))
#define	SA	struct sockaddr

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

ssize_t RecvFrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen)
{
    ssize_t n = recvfrom(sockfd, buf, len, flags, src_addr, addrlen);
    if (n == -1) {
        int errorCode = errno;
        throw std::runtime_error(ERROR_MESSAGE(errorCode));
    }
    return n;
}

ssize_t SendTo(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen)
{
    ssize_t n = sendto(sockfd, buf, len, flags, dest_addr, addrlen);
    if (n == -1) {
        int errorCode = errno;
        throw std::runtime_error(ERROR_MESSAGE(errorCode));
    }
    return n;
}

void DgEcho(int sockfd, SA *pcliaddr, socklen_t clilen)
{
    const size_t MAXLINE = 0xffff - 8; 
    char mesg[MAXLINE];

    for ( ; ; ) {
        socklen_t len = clilen;
        int n = RecvFrom(sockfd, mesg, MAXLINE, 0, pcliaddr, &len);

        SendTo(sockfd, mesg, n, 0, pcliaddr, len);
    }
}

int main(int argc, char **argv)
{
    try {
        if (argc != 2) {
            throw std::runtime_error("wrong argc");
        }

        int serverPort = std::stoi(argv[1]);

        int sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

        Bind(sockfd, "0.0.0.0", serverPort);

        struct sockaddr_in cliaddr;
        DgEcho(sockfd, (SA *) &cliaddr, sizeof(cliaddr));

    } catch (const std::exception& ex) {
        std::cout << ex.what() << std::endl;
    }

    return 0;
}

#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <iostream>
#include <string>

#define ERROR_MESSAGE(code) (std::string(__FUNCTION__) + ", " + strerror(code))
#define	SA	struct sockaddr
const size_t MAXLINE = 0xffff - 8; 
#define CONNECT

int Socket(int domain, int type, int protocol)
{
    int fd = socket(domain, type, protocol);
    if (fd == -1) {
        int errorCode = errno;
        throw std::runtime_error(ERROR_MESSAGE(errorCode));
    }
    return fd;
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

void DgCli(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen)
{
    char sendline[MAXLINE];
    char recvline[MAXLINE + 1];

    while (fgets(sendline, MAXLINE, fp) != nullptr) {

        SendTo(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);

        ssize_t n = RecvFrom(sockfd, recvline, MAXLINE, 0, NULL, NULL);

        recvline[n] = 0;
        fputs(recvline, stdout);
    }
}

void SetServAddr(const std::string servIp, const int servPort, sockaddr_in& servaddr)
{
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;

    if (inet_aton(servIp.c_str(), &(servaddr.sin_addr)) == 0) {
        int errorCode = errno;
        throw std::runtime_error(ERROR_MESSAGE(errorCode));
    }
    servaddr.sin_port = htons(servPort);
}

int main(int argc, char **argv)
{
    try {
        if(argc != 3) {
            throw std::runtime_error("wrong argc");
        }

        std::string servIp{ argv[1] };
        int servPort = std::stoi(argv[2]);
        struct sockaddr_in servaddr;
        SetServAddr(servIp, servPort, servaddr);

        int sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

        DgCli(stdin, sockfd, (SA*)&servaddr, sizeof(servaddr));

    } catch (const std::exception& ex) {
        std::cout << ex.what() << std::endl;
    }
    return 0;
}
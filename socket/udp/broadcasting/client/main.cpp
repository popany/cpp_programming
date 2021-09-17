#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <iostream>
#include <string>
#include <vector>
#include <sys/un.h>  // define sockaddr_un 

#define ERROR_MESSAGE(code) (std::string(__FUNCTION__) + ", " + strerror(code))
#define	SA	struct sockaddr
const size_t MAXLINE = 0xffff - 8; 

std::string SockNtopHost(const sockaddr *sa, socklen_t salen)
{
    static char str[128];        /* Unix domain is largest */

    switch (sa->sa_family) {
    case AF_INET: {
        sockaddr_in *sin = (sockaddr_in *) sa;
        if (inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL) {
            int errorCode = errno;
            throw std::runtime_error(ERROR_MESSAGE(errorCode));
        }
        return str;
    }

    case AF_INET6: {
        sockaddr_in6 *sin6 = (sockaddr_in6 *) sa;
        if (inet_ntop(AF_INET6, &sin6->sin6_addr, str, sizeof(str)) == NULL) {
            int errorCode = errno;
            throw std::runtime_error(ERROR_MESSAGE(errorCode));
        }
        return str;
    }

    case AF_UNIX: {
        sockaddr_un *unp = (sockaddr_un *) sa;

            /* OK to have no pathname bound to the socket: happens on
               every connect() unless client calls bind() first. */
        if (unp->sun_path[0] == 0) {
            strcpy(str, "(no pathname bound)");
        } else {
            snprintf(str, sizeof(str), "%s", unp->sun_path);
        }
        return str;
    }

#ifdef HAVE_SOCKADDR_DL_STRUCT
    case AF_LINK: {
        struct sockaddr_dl *sdl = (struct sockaddr_dl *) sa;

        if (sdl->sdl_nlen > 0) {
            snprintf(str, sizeof(str), "%*s", sdl->sdl_nlen, &sdl->sdl_data[0]);
        } else {
            snprintf(str, sizeof(str), "AF_LINK, index=%d", sdl->sdl_index);
        }
        return str;
    }
#endif
    default:
        snprintf(str, sizeof(str), "SockNtopHost: unknown AF_xxx: %d, len %d", sa->sa_family, salen);
        throw std::runtime_error(str);
    }
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

void Setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen)
{
    if (setsockopt(sockfd, level, optname, optval, optlen) == -1) {
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

void DgCli(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen)
{
    char sendline[MAXLINE];
    char recvline[MAXLINE + 1];
    std::vector<char> replyAddrBuf(servlen, 0);
    SA* preplayAddr = (SA*)&replyAddrBuf[0];

    const int on = 1;
    Setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));

    while (fgets(sendline, MAXLINE, fp) != nullptr) {

        SendTo(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);

        socklen_t len = servlen;
        ssize_t n = RecvFrom(sockfd, recvline, MAXLINE, 0, preplayAddr, &len);

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

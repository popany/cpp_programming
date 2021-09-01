#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <iostream>
#include <string>

#define ERROR_MESSAGE(code) (std::string(__FUNCTION__) + ", " + strerror(code))
#define	SA	struct sockaddr

static const int SERV_PORT = 31001;

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

void DgEcho()
{
    ;
}

int main(int argc, char **argv)
{
    try {
        int sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

        Bind(sockfd, INADDR_ANY, SERV_PORT);

        struct sockaddr_in cliaddr;
        DgEcho(sockfd, (SA *) &cliaddr, sizeof(cliaddr));

    } catch (const std::exception& ex) {
        std::cout << ex.what() << std::endl;
    }

    return 0;
}

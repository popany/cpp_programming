#pragma once

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdexcept>
#include <string>
#include "log.h"

void InitWinsock()
{
	WSADATA wsaData;

    // The WSAStartup function initiates use of the Winsock DLL by a process.
    // https://docs.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-wsastartup
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        throw std::runtime_error("WSAStartup failed with error: " + std::to_string(iResult));
    }
}

void CloseSocket(SOCKET s)
{
    if (SOCKET_ERROR == closesocket(s)) {
        int code = WSAGetLastError();
        LogError("closesocket failed, code(", code, ")");
    }
}

SOCKET CreateListenSocket(int port)
{
    // Parameters
    // https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-wsasocketa
    //   `af`
    //     The values currently supported are `AF_INET` or `AF_INET6`, which are the Internet address family formats for IPv4 and IPv6.
    //   `type`
    //     `SOCK_STREAM` - A socket type that provides sequenced, reliable, two-way, connection-based byte streams with an OOB data transmission mechanism.
    //                     This socket type uses the Transmission Control Protocol (TCP) for the Internet address family (`AF_INET` or `AF_INET6`).
    //   `protocol`
    //     If a value of 0 is specified, the caller does not wish to specify a protocol and the service provider will choose the protocol to use.
    //     `IPPROTO_TCP` - The Transmission Control Protocol(TCP). This is a possible value when the af parameter is `AF_INET` or `AF_INET6` and the type
    //                     parameter is `SOCK_STREAM`.
    //   `lpProtocolInfo`
    //     A pointer to a `WSAPROTOCOL_INFO` structure that defines the characteristics of the socket to be created. If this parameter is not `NULL`, the
    //     socket will be bound to the provider associated with the indicated `WSAPROTOCOL_INFO` structure.
    //   `g`
    //     An existing socket group ID or an appropriate action to take when creating a new socket and a new socket group.
    //     `0` - No group operation is performed.
    //   `dwFlags`
    //     A set of flags used to specify additional socket attributes.
    //     A combination of these flags may be set, although some combinations are not allowed.
    //     `WSA_FLAG_OVERLAPPED` - Create a socket that supports overlapped I/O operations.
    SOCKET listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (listenSocket == INVALID_SOCKET) {
        int code = WSAGetLastError();
        throw std::runtime_error(std::string(__FUNCTION__) + ", WSASocket() failed, code(" + std::to_string(code) + ")");
    }

    sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port);
    if (SOCKET_ERROR == bind(listenSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress))) {
        int code = WSAGetLastError();
		CloseSocket(listenSocket);
        throw std::runtime_error(std::string(__FUNCTION__) + ", bind() failed, code(" + std::to_string(code) + ")");
    }

    if (SOCKET_ERROR == listen(listenSocket, SOMAXCONN)) {
        int code = WSAGetLastError();
		CloseSocket(listenSocket);
        throw std::runtime_error(std::string(__FUNCTION__) + ", listen() failed, code(" + std::to_string(code) + ")");
    }

    return listenSocket;
}


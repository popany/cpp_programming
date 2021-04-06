#pragma once

#define _WINSOCKAPI_
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

void InitWinsock();
void CloseSocket(SOCKET s);
SOCKET CreateListenSocket(int port);


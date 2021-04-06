// https://docs.microsoft.com/en-us/windows/win32/fileio/i-o-completion-ports
// https://www.codeproject.com/articles/13382/a-simple-application-using-i-o-completion-ports-an
// https://github.com/Microsoft/Windows-classic-samples/blob/master/Samples/Win7Samples/netds/winsock/iocp/server/IocpServer.Cpp

#define WIN32_LEAN_AND_MEAN
// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

#include <iostream>
#include <string>
#include <stdexcept>
#include <thread>
#include <chrono>

#include "socket.h"
#include "log.h"
#include "iocp.h"

int main()
{
    try {
        InitWinsock();

        auto t = std::thread([]() { IOCP::getInstance().start(10001, 4, 5); });

        while (true) {
            std::string s;
            std::cin >> s;
            if (s == "exit") {
                break;
            }
        }
        
        IOCP::getInstance().stop();

        t.join();
		return 0;
    }
    catch (const std::exception& e) {
        LogError("exception: ", e.what());
    }
}
#include "server.h"
#include "logger.h"
#include "config/server_config.h"
#include <signal.h>
#include <thread>

void HandleSignal(int signum)
{
    if (signum == SIGINT || signum == SIGTERM) {
        std::thread t([=]() {
            LOG_INFO("signal({}) received", signum);
            Server::getInstance().stop();
        });
        t.detach();
    }
}

void RegisterSignalHandler()
{
    signal(SIGINT, HandleSignal);
    signal(SIGTERM, HandleSignal);
}

int main()
{
    InitLogger();
    SetLogLevel(SERVER_CONFIG.GET_LOG_LEVEL());
    RegisterSignalHandler();
    Server::getInstance().start();

    return 0;
}

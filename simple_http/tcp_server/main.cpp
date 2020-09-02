#include <exception>
#include <functional>
#include <string>
#include "log.h"
#include "utils.h"
#include "config.h"
#include "server.h"
#include "connection.h"
#include "http_request.h"

int main()
{
    Logger::GetInstance().SetLogLevel(Config::GetInstance().logLevel);

    try {
        SetExitCondition();

        Server server(Config::GetInstance().port, Config::GetInstance().epEventCount);
        ConnectionManager connectionMgr([](std::function<void(std::string)> f){ return std::shared_ptr<RequestHandler>(new HttpRequestHandler(f)); });
        server.RegisterReportFunctions(
            std::bind(&ConnectionManager::NewConnection, &connectionMgr, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
            std::bind(&ConnectionManager::CanRead, &connectionMgr, std::placeholders::_1),
            std::bind(&ConnectionManager::CanWrite, &connectionMgr, std::placeholders::_1),
            std::bind(&ConnectionManager::ErrorOccurred, &connectionMgr, std::placeholders::_1),
            std::bind(&ConnectionManager::TimeToExit, &connectionMgr, std::placeholders::_1)
        );

        server.Start();
        connectionMgr.Close();
    } catch (const std::exception& e) {
        LogError("exception: ", e.what());
    }

    return 0;
}


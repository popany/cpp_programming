#include <stdint.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <map>
#include <memory>
#include <mutex>
#include "log.h"
#include "utils.h"
#include "config.h"
#include "server.h"
#include "connection.h"

int main()
{
    Logger::GetInstance().SetLogLevel(Config::GetInstance().logLevel);

    try {
        SetExitCondition();

        Server server(Config::GetInstance().port, Config::GetInstance().epEventCount);
        ConnectionManager connectionMgr;
        server.RegisterReportFunctions(
            std::bind(&ConnectionManager::NewConnection, &connectionMgr, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
            std::bind(&ConnectionManager::CanRead, &connectionMgr, std::placeholders::_1),
            std::bind(&ConnectionManager::CanWrite, &connectionMgr, std::placeholders::_1),
            std::bind(&ConnectionManager::ErrorOccurred, &connectionMgr, std::placeholders::_1),
            std::bind(&ConnectionManager::TimeToExit, &connectionMgr, std::placeholders::_1)
        );

        server.Start();
    } catch (const std::exception& e) {
        LogError("exception: ", e.what());
    }

    return 0;
}


#pragma once
#include <string>
#include <map>
#include <stdexcept>
#include <memory>
#include <functional>
#include "utils.h"
#include "log.h"
#include "common_types.h"
#include "socket_handler.h"
#include "request_handler.h"

class ConnectionMap;

class ConnectionManager
{
    std::shared_ptr<ConnectionMap> connectionMap;

    void ReleaseConnection(uint64_t id);
    void VerifyConnectionExist(uint64_t id);
    void VerifyConnectionNotExist(uint64_t id);
    std::function<std::shared_ptr<RequestHandler>()> createRequestHandler;

public:
    ConnectionManager(std::function<std::shared_ptr<RequestHandler>()> createRequestHandler);
    void NewConnection(uint64_t id, ClientInfo clientInfo, SocketHandler socketHandler);
    void CanRead(uint64_t id);
    void CanWrite(uint64_t id);
    void ErrorOccurred(uint64_t id);
    void TimeToExit(uint64_t id);
};

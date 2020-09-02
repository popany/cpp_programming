#pragma once
#include <string>
#include <map>
#include <stdexcept>
#include <memory>
#include <functional>
#include <mutex>
#include "utils.h"
#include "log.h"
#include "common_types.h"
#include "socket_handler.h"
#include "request_handler.h"

class ConnectionMap;
class WriteCache;
class Connection;
class ConnectionQueue;

typedef ConnectionQueue ReadQueue;
typedef ConnectionQueue WriteQueue;

class ConnectionManager
{
    enum
    {
        DEFAULT_PROCESS_EVENT_THREAD_COUNT = 2,
    };
    std::shared_ptr<ConnectionMap> connectionMap;
    std::shared_ptr<ReadQueue> readQueue;
    std::shared_ptr<WriteQueue> writeQueue;
    std::shared_ptr<WriteCache> writeCache;
    std::mutex protectIsReading;
    std::mutex protectIsWriting;
    EventNotifier readInQueueHappened;
    EventNotifier writeInQueueHappened;
    bool closed;

    bool NeedInQueueRead(const std::shared_ptr<Connection>& connection);
    bool NeedReInQueueRead(const std::shared_ptr<Connection>& connection);
    void CheckToInQueueRead(const std::shared_ptr<Connection>& connection);
    bool NeedInQueueWrite(const std::shared_ptr<Connection>& connection);
    bool NeedReInQueueWrite(const std::shared_ptr<Connection>& connection);
    void CheckToInQueueWrite(const std::shared_ptr<Connection>& connection);
    void AddToWriteCache(uint64_t connectionId, std::string contentToWrite);
    void ReleaseConnection(uint64_t id);
    void ProcessRead(int id);
    void ProcessWrite(int id);
    std::function<std::shared_ptr<RequestHandler>(std::function<void(std::string)>)> createRequestHandler;

public:
    ConnectionManager(std::function<std::shared_ptr<RequestHandler>(std::function<void(std::string)>)> createRequestHandler, int processEventThreadCount = DEFAULT_PROCESS_EVENT_THREAD_COUNT);
    void NewConnection(uint64_t id, ClientInfo clientInfo, SocketHandler socketHandler);
    void CanRead(uint64_t id);
    void CanWrite(uint64_t id);
    void ErrorOccurred(uint64_t id);
    void TimeToExit(uint64_t id);
    void Close();
};

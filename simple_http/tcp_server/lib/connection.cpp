#include <queue>
#include <atomic>
#include <thread>
#include <functional>
#include <utility>
#include "connection.h"
#include "log.h"
#include "http_request.h"

class Connection
{
    uint64_t id;
    ClientInfo clientInfo;
    SocketHandler socketHandler;
    std::function<void()> release;

    std::atomic_int isReadable;
    std::atomic_int isWritable;
    std::atomic_int isToClose;
    std::atomic_int isError;
    bool isClosed;
    bool isReading;
    bool isWriting;

    std::shared_ptr<RequestHandler> requestHandler;
    std::string response;

public:
    Connection(uint64_t id, ClientInfo clientInfo, SocketHandler socketHandler, std::shared_ptr<RequestHandler> requestHandler, std::function<void()> release):
        id(id),
        clientInfo(clientInfo),
        socketHandler(socketHandler),
        requestHandler(requestHandler),
        release(release),
        isReadable(0),
        isWritable(0),
        isToClose(0),
        isError(0),
        isClosed(false),
        isReading(false),
        isWriting(false)
    {
    }

    uint64_t GetConnectionId()
    {
        return id;
    }

    std::string GetClientInfo()
    {
        return "<connection id: " + std::to_string(id) + ">(" + clientInfo.ip + ":" + std::to_string(clientInfo.port) + ")";
    }

    void SetReadable()
    {
        isReadable++;
    }

    void SetWritable()
    {
        isWritable++;
    }

    void SetError()
    {
        isError++;
    }

    void SetToClose()
    {
        isToClose++;
    }

    bool ReadEventHappend()
    {
        return isReadable | isError | isToClose;
    }

    bool WriteEventHappend()
    {
        return isWritable | isError | isToClose;
    }

    bool IsReading()
    {
        return isReading;
    }

    void SetReading()
    {
        isReading = true;
    }

    void UnSetReading()
    {
        isReading = false;
    }

    void Read()
    {
        if (!isReadable) {
            return;
        }
        isReadable--;

        std::string s = socketHandler.Read();
        LogDebug(GetClientInfo(), ". read: \"", s, "\"");

        requestHandler->Append(s);

        if (requestHandler->CheckIntegrity()) {
            requestHandler->ReadCompleteCallback(0);
        }
    }

    bool IsWriting()
    {
        return isWriting;
    }

    void SetWriting()
    {
        isWriting = true;
    }

    void UnSetWriting()
    {
        isWriting = false;
    }

    void SetResponse(std::string s)
    {
        response += s;
    }

    void Write()
    {
        if (!isWritable) {
            return;
        }
        isWritable--;

        if (response.empty()) {
            return;
        }

        response = socketHandler.Write(response);

        if (response.empty()) {
            requestHandler->WriteCompleteCallback(0);
        }
    }

    void Close()
    {
        if (isClosed) {
            throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + ", Connection is closed");
        }
        socketHandler.Close();
        release();
        isClosed = true;
        LogDebug(GetClientInfo());
    }
};

class ConnectionMap
{
    std::map<uint64_t, std::shared_ptr<Connection>> connections;
    std::mutex m;

public:
    std::shared_ptr<Connection> GetValue(uint64_t id)
    {
        std::lock_guard<std::mutex> lk(m);
        if (connections.count(id) == 0) {
            throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + ", id(" + std::to_string(id) + ") not exist");
        }
        return connections[id];
    }

    void Add(uint64_t id, std::shared_ptr<Connection> connection)
    {
        std::lock_guard<std::mutex> lk(m);
        if (connections.count(id) != 0) {
            throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + ", id(" + std::to_string(id) + ") exist");
        }
        connections[id] = connection;
    }

    void Erase(uint64_t id)
    {
        std::lock_guard<std::mutex> lk(m);
        if (connections.count(id) == 0) {
            throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + ", id(" + std::to_string(id) + ") not exist");
        }
        connections.erase(id);
    }

    size_t Size()
    {
        std::lock_guard<std::mutex> lk(m);
        return connections.size();
    }
};

class ConnectionQueue
{
    std::queue<std::shared_ptr<Connection>> connections;
    std::mutex m;

public:
    ConnectionQueue()
    {}

    void InQueue(const std::shared_ptr<Connection>& connection)
    {
        std::lock_guard<std::mutex> lk(m);
        connections.push(connection);
    }

    bool DeQueue(std::shared_ptr<Connection>& connection)
    {
        std::lock_guard<std::mutex> lk(m);
        if (connections.empty()) {
            return false;
        }
        connection = connections.front();
        connections.pop();
        return true;
    }
};

class WriteCache
{
    std::map<uint64_t, std::string> cache;
    std::mutex m;

public:
    WriteCache()
    {}

    void Add(uint64_t connectionId, std::string contentToWrite)
    {
        std::lock_guard<std::mutex> lk(m);
        if (cache.count(connectionId) > 0) {
            cache[connectionId] += contentToWrite;
            return;
        }
        cache[connectionId] = contentToWrite;
    }

    std::string Get(uint64_t connectionId)
    {
        std::lock_guard<std::mutex> lk(m);
        std::string tmp{};
        if (cache.count(connectionId) > 0) {
            tmp.swap(cache[connectionId]);
        }
        return tmp;
    }

    bool HasContentToWrite(uint64_t connectionId)
    {
        std::lock_guard<std::mutex> lk(m);
        if (cache.count(connectionId) > 0) {
            return !cache[connectionId].empty();
        }
        return false;
    }
};

ConnectionManager::ConnectionManager(std::function<std::shared_ptr<RequestHandler>(std::function<void(std::string)>)> createRequestHandler, int processEventThreadCount) :
    createRequestHandler(createRequestHandler),
    connectionMap(std::make_shared<ConnectionMap>()),
    readQueue(std::make_shared<ReadQueue>()),
    writeQueue(std::make_shared<WriteQueue>()),
    writeCache(std::make_shared<WriteCache>())
{
    for (int i = 0; i < processEventThreadCount; i++) {
        std::thread tr([=] { ProcessRead(i); });
        tr.detach();
        std::thread tw([=] { ProcessWrite(i); });
        tw.detach();
    }
}

void ConnectionManager::ReleaseConnection(uint64_t id)
{
    try {
        connectionMap->Erase(id);
    } catch (const std::exception& ex) {
        LogError("exception: ", ex.what());
    }
}

void ConnectionManager::NewConnection(uint64_t id, ClientInfo clientInfo, SocketHandler socketHandler)
{
    try {
        std::shared_ptr<Connection> connection = std::make_shared<Connection>(
            id,
            clientInfo,
            socketHandler,
            createRequestHandler(std::bind(&ConnectionManager::AddToWriteCache, this, id, std::placeholders::_1)),
            std::bind(&ConnectionManager::ReleaseConnection, this, id)
        );
        connectionMap->Add(id, connection);
        LogDebug(connection->GetClientInfo(), ", connection count: ", connectionMap->Size());
    } catch (const std::exception& ex) {
        LogError("exception: ", ex.what());
    }
}

void ConnectionManager::CanRead(uint64_t id)
{
    try {
        auto connection = connectionMap->GetValue(id);
        LogDebug(connection->GetClientInfo());
        connection->SetReadable();
        std::atomic_thread_fence(std::memory_order_acquire);
        CheckToInQueueRead(connection);
    } catch (const std::exception& ex) {
        LogError("exception: ", ex.what());
    }
}

void ConnectionManager::CanWrite(uint64_t id)
{
    try {
        auto connection = connectionMap->GetValue(id);
        LogDebug(connection->GetClientInfo());
        connection->SetWritable();
        std::atomic_thread_fence(std::memory_order_acquire);
        CheckToInQueueWrite(connection);
    } catch (const std::exception& ex) {
        LogError("exception: ", ex.what());
    }
}

void ConnectionManager::ErrorOccurred(uint64_t id)
{
    try {
        auto connection = connectionMap->GetValue(id);
        LogDebug(connection->GetClientInfo());
        connection->SetError();
        std::atomic_thread_fence(std::memory_order_acquire);
        CheckToInQueueRead(connection);
        CheckToInQueueWrite(connection);
    } catch (const std::exception& ex) {
        LogError("exception: ", ex.what());
    }
}

void ConnectionManager::TimeToExit(uint64_t id)
{
    try {
        auto connection = connectionMap->GetValue(id);
        LogDebug(connection->GetClientInfo());
        connection->SetToClose();
        std::atomic_thread_fence(std::memory_order_acquire);
        CheckToInQueueRead(connection);
        CheckToInQueueWrite(connection);
    } catch (const std::exception& ex) {
        LogError("exception: ", ex.what());
    }
}

bool ConnectionManager::NeedInQueueRead(const std::shared_ptr<Connection>& connection)
{
    std::lock_guard<std::mutex> lk(protectIsReading);
    if (connection->ReadEventHappend() && !connection->IsReading()) {
        connection->SetReading();
        return true;
    }
    return false;
}

bool ConnectionManager::NeedReInQueueRead(const std::shared_ptr<Connection>& connection)
{
    std::lock_guard<std::mutex> lk(protectIsReading);
    if (!connection->IsReading()) {
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__));
    }
    if (!connection->ReadEventHappend()) {
        connection->UnSetReading();
        return false;
    }
    return true;
}

void ConnectionManager::CheckToInQueueRead(const std::shared_ptr<Connection>& connection)
{
    if (NeedInQueueRead(connection)) {
        readQueue->InQueue(connection);
    }
}

void ConnectionManager::ProcessRead(int id)
{
    LogInfo("start, id: ", id);

    while (!closed) {
        readInQueueHappened.Wait();

        for (;;) {
            std::shared_ptr<Connection> connection;
            if (!readQueue->DeQueue(connection)) {
                break;
            }

            connection->Read();
        
            if (NeedReInQueueRead(connection)) {
                readQueue->InQueue(connection);
            }
        }
    }
}

void ConnectionManager::AddToWriteCache(uint64_t connectionId, std::string contentToWrite)
{
    writeCache->Add(connectionId, contentToWrite);
    auto connection = connectionMap->GetValue(connectionId);
    if (NeedInQueueWrite(connection)) {
        writeQueue->InQueue(connection);
    }
}

bool ConnectionManager::NeedInQueueWrite(const std::shared_ptr<Connection>& connection)
{
    std::lock_guard<std::mutex> lk(protectIsWriting);
    if (connection->WriteEventHappend() && writeCache->HasContentToWrite(connection->GetConnectionId()) && !connection->IsWriting()) {
        connection->SetWriting();
        return true;
    }
    return false;
}

bool ConnectionManager::NeedReInQueueWrite(const std::shared_ptr<Connection>& connection)
{
    std::lock_guard<std::mutex> lk(protectIsWriting);
    if (!connection->IsWriting()) {
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__));
    }
    if (!connection->WriteEventHappend() || !writeCache->HasContentToWrite(connection->GetConnectionId())) {
        connection->UnSetWriting();
        return false;
    }
    return true;
}

void ConnectionManager::CheckToInQueueWrite(const std::shared_ptr<Connection>& connection)
{
    if (NeedInQueueWrite(connection)) {
        writeQueue->InQueue(connection);
    }
}

void ConnectionManager::ProcessWrite(int id)
{
    LogInfo("start, id: ", id);

    while (!closed) {
        writeInQueueHappened.Wait();

        for (;;) {
            std::shared_ptr<Connection> connection;
            if (!writeQueue->DeQueue(connection)) {
                break;
            }
            std::string contentToWrite = writeCache->Get(connection->GetConnectionId());
            connection->SetResponse(contentToWrite);
            connection->Write();
            
            if (NeedReInQueueWrite(connection)) {
                writeQueue->InQueue(connection);
            }
        }
    }
}

void ConnectionManager::Close()
{
    closed = true;
}

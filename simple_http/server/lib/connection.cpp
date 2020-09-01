#include "connection.h"
#include "log.h"
#include "http_request.h"

class Connection
{
    uint64_t id;
    ClientInfo clientInfo;
    SocketHandler socketHandler;
    std::function<void()> release;

    bool isReadable;
    bool isWritable;
    bool isToClose;
    bool isError;
    bool isClosed;

    std::shared_ptr<RequestHandler> requestHandler;
    std::string response;

public:
    Connection(uint64_t id, ClientInfo clientInfo, SocketHandler socketHandler, std::shared_ptr<RequestHandler> requestHandler, std::function<void()> release):
        id(id),
        clientInfo(clientInfo),
        socketHandler(socketHandler),
        requestHandler(requestHandler),
        release(release),
        isReadable(false),
        isWritable(false),
        isToClose(false),
        isError(false),
        isClosed(false)
    {
    }

    std::string GetClientInfo()
    {
        return "<connection id: " + std::to_string(id) + ">(" + clientInfo.ip + ":" + std::to_string(clientInfo.port) + ")";
    }

    void SetReadable()
    {
        isReadable = true;
    }

    void SetWritable()
    {
        isWritable = true;
    }

    void SetError()
    {
        isError = true;
    }

    void SetToClose()
    {
        isToClose = true;
    }

    void Read()
    {
        if (!isReadable) {
            return;
        }
        isReadable = false;

        std::string s = socketHandler.Read();
        LogDebug(GetClientInfo(), ". read: \"", s, "\"");

        requestHandler->Append(s);

        if (requestHandler->CheckIntegrity()) {
            requestHandler->ReadCompleteCallback(0);
        }
    }

    void Write(std::string s)
    {
        if (!isWritable) {
            return;
        }
        isWritable = false;

        response += s;
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
        return connections[id];
    }

    void Add(uint64_t id, std::shared_ptr<Connection> connection)
    {
        std::lock_guard<std::mutex> lk(m);
        connections[id] = connection;
    }

    void Erase(uint64_t id)
    {
        std::lock_guard<std::mutex> lk(m);
        connections.erase(id);
    }

    size_t Count(uint64_t id)
    {
        std::lock_guard<std::mutex> lk(m);
        return connections.count(id);
    }

    size_t Size()
    {
        std::lock_guard<std::mutex> lk(m);
        return connections.size();
    }
};

void ConnectionManager::ReleaseConnection(uint64_t id)
{
    VerifyConnectionExist(id);
    connectionMap->Erase(id);
}

void ConnectionManager::VerifyConnectionExist(uint64_t id)
{
    if (connectionMap->Count(id) == 0) {
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__));
    }
}

void ConnectionManager::VerifyConnectionNotExist(uint64_t id)
{
    if (connectionMap->Count(id) != 0) {
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__));
    }
}

ConnectionManager::ConnectionManager(std::function<std::shared_ptr<RequestHandler>()> createRequestHandler) :
    createRequestHandler(createRequestHandler),
    connectionMap(std::make_shared<ConnectionMap>())
{}

void ConnectionManager::NewConnection(uint64_t id, ClientInfo clientInfo, SocketHandler socketHandler)
{
    VerifyConnectionNotExist(id);
    std::shared_ptr<Connection> connection = std::make_shared<Connection>(
        id,
        clientInfo,
        socketHandler,
        createRequestHandler(),
        std::bind(&ConnectionManager::ReleaseConnection, this, id)
    );

    connectionMap->Add(id, connection);
    LogDebug(connection->GetClientInfo(), ", connection count: ", connectionMap->Size());
}

void ConnectionManager::CanRead(uint64_t id)
{
    VerifyConnectionExist(id);
    auto connection = connectionMap->GetValue(id);
    LogDebug(connection->GetClientInfo());

    connection->SetReadable();
}

void ConnectionManager::CanWrite(uint64_t id)
{
    VerifyConnectionExist(id);
    auto connection = connectionMap->GetValue(id);
    LogDebug(connection->GetClientInfo());

    connection->SetWritable();
}

void ConnectionManager::ErrorOccurred(uint64_t id)
{
    VerifyConnectionExist(id);
    auto connection = connectionMap->GetValue(id);
    LogDebug(connection->GetClientInfo());

    connection->SetError();
}

void ConnectionManager::TimeToExit(uint64_t id)
{
    VerifyConnectionExist(id);
    auto connection = connectionMap->GetValue(id);
    LogDebug(connection->GetClientInfo());

    connection->SetToClose();
}

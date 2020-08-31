#include "connection.h"
#include "log.h"
#include "http_request.h"

class Connection
{
    uint64_t id;
    ClientInfo clientInfo;

    HttpRequest request;
    std::string response;

    bool isReadable;
    bool isWritable;
    bool isClosed;
    bool processComplete;

    SocketHandler socketHandler;

    std::function<void()> release;

    bool ClientClosed()
    {
        return socketHandler.IsEof();
    }

public:
    Connection(uint64_t id, ClientInfo clientInfo, SocketHandler socketHandler, std::function<void()> release):
        id(id),
        clientInfo(clientInfo),
        socketHandler(socketHandler),
        release(release),
        isReadable(false),
        isWritable(false),
        isClosed(false),
        processComplete(false)
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

    void Process()
    {
        request.PrintRequest();
        std::string receivedMsg = request.GetMessageBody();
        std::string responseMsg = "hello\r\n";
        
        response = "HTTP/1.1 200 OK\r\n";
        response += "Context-Length: " +  std::to_string(responseMsg.size()) + "\r\n";
        response += "\r\n";
        response += responseMsg;

        processComplete = true;
    }

    bool CanClose()
    {
        if (!response.empty()) {
            return false;
        }

        if (ClientClosed()) {
            return true;
        }

        return processComplete;
    }

    void Read()
    {
        if (!isReadable) {
            return;
        }

        std::string s = socketHandler.Read();
        LogDebug(GetClientInfo(), ". read: \"", s, "\"");

        request.Append(s);

        if (request.AllReceived()) {
            Process();
        }

        isReadable = false;
        Write();
    }

    void Write()
    {
        if (!isWritable) {
            return;
        }

        response = socketHandler.Write(response);

        isWritable = false;
        
        if (CanClose()) {
            Close();
        }
    }

    void Close()
    {
        if (isClosed) {
            throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + ", Connection is closed");
        }
        socketHandler.Close();
        release();
        LogDebug(GetClientInfo());
        isClosed = true;
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

ConnectionManager::ConnectionManager():
    connectionMap(std::make_shared<ConnectionMap>())
{}

void ConnectionManager::NewConnection(uint64_t id, ClientInfo clientInfo, SocketHandler socketHandler)
{
    VerifyConnectionNotExist(id);
    std::shared_ptr<Connection> connection = std::make_shared<Connection>(id, clientInfo, socketHandler, std::bind(&ConnectionManager::ReleaseConnection, this, id));

    connectionMap->Add(id, connection);
    LogDebug(connection->GetClientInfo(), ", connection count: ", connectionMap->Size());
}

void ConnectionManager::CanRead(uint64_t id)
{
    VerifyConnectionExist(id);
    auto connection = connectionMap->GetValue(id);
    LogDebug(connection->GetClientInfo());

    connection->SetReadable();
    connection->Read();
}

void ConnectionManager::CanWrite(uint64_t id)
{
    VerifyConnectionExist(id);
    auto connection = connectionMap->GetValue(id);
    LogDebug(connection->GetClientInfo());

    connection->SetWritable();
    connection->Write();
}

void ConnectionManager::ErrorOccurred(uint64_t id)
{
    VerifyConnectionExist(id);
    auto connection = connectionMap->GetValue(id);
    LogDebug(connection->GetClientInfo());

    connection->Close();
}

void ConnectionManager::TimeToExit(uint64_t id)
{
    VerifyConnectionExist(id);
    auto connection = connectionMap->GetValue(id);
    LogDebug(connection->GetClientInfo());

    connection->Close();
}

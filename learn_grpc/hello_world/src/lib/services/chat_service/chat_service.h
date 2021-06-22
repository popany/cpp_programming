#pragma once

#include "logger.h"
#include "chat.grpc.pb.h"

class ChatServiceImpl : public ChatService::Service
{
    ChatServiceImpl();
public:
    ChatServiceImpl(const ChatServiceImpl&) = delete;
    void operator=(const ChatServiceImpl&) = delete;

    static ChatServiceImpl& getInstance();
    grpc::Status greet(grpc::ServerContext* context, const ClientWords* request, ServerWords* response) override;
    grpc::Status listen(grpc::ServerContext* context, const ClientWords* request, grpc::ServerWriter<ServerWords>* writer);
    grpc::Status speak(grpc::ServerContext* context, grpc::ServerReader<ClientWords>* reader, ServerWords* response);
    grpc::Status talk(grpc::ServerContext* context, grpc::ServerReaderWriter<ServerWords, ClientWords>* stream);
};

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
};

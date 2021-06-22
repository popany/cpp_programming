#pragma once

#include <grpcpp/grpcpp.h>
#include "chat.grpc.pb.h"
#include "client_proactor.h"

class AsyncChatClient
{
    std::unique_ptr<ChatService::Stub> stub;

    void processSayChatResponse(void* token);
    void processSayChatAgainResponse(void* token);

public:
    AsyncChatClient(std::shared_ptr<grpc::Channel> channel);
    void greet();
    void listen();
    void speak();
    void talk();
};

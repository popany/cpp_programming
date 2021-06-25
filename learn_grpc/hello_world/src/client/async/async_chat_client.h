#pragma once

#include <grpcpp/grpcpp.h>
#include "chat.grpc.pb.h"
#include "client_proactor.h"

template <typename Msg>
class AsyncChatWriter
{
public:
    virtual void write(Msg&& msg) = 0;
    virtual void close() = 0;
};

class AsyncChatClient
{
    std::unique_ptr<ChatService::Stub> stub;

    void processSayChatResponse(void* token);
    void processSayChatAgainResponse(void* token);

public:
    AsyncChatClient(std::shared_ptr<grpc::Channel> channel);
    void greet();
    void listen();
    AsyncChatWriter<const std::string&>& speak();
    AsyncChatWriter<const std::string&>& talk();
};

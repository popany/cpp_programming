#pragma once

#include <grpcpp/grpcpp.h>
#include "chat.grpc.pb.h"
#include <memory>

class ChatClient
{
    std::unique_ptr<ChatService::Stub> stub;
public:
    ChatClient(std::shared_ptr<grpc::Channel> channel);
    void greet();
    void listen();
    void speek();
    void talk();
};

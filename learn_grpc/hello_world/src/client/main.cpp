#include "logger.h"
#include "config/client_config.h"
#include "hello_client.h"
#include "goodbye_client.h"
#include "chat_client.h"
#include "async_hello_client.h"
#include "async_goodbye_client.h"
#include "async_chat_client.h"
#include <grpcpp/grpcpp.h>

void UseSyncClient(const std::string& serverAddress)
{
    HelloClient helloClient(grpc::CreateChannel(serverAddress, grpc::InsecureChannelCredentials()));
    helloClient.sayHello();
    helloClient.sayHelloAgain();

    GoodbyeClient goodbyeClient(grpc::CreateChannel(serverAddress, grpc::InsecureChannelCredentials()));
    goodbyeClient.sayGoodbye();
    goodbyeClient.sayGoodbyeAgain();

    ChatClient chatClient(grpc::CreateChannel(serverAddress, grpc::InsecureChannelCredentials()));
    chatClient.greet();
    chatClient.listen();
    chatClient.speak();
    chatClient.talk();
}

void UseAsyncClient(const std::string& serverAddress)
{
    AsyncHelloClient helloClient(grpc::CreateChannel(serverAddress, grpc::InsecureChannelCredentials()));
    helloClient.startThreadPool();
    for (int i = 0; i < CLIENT_CONFIG.GET_GRPC_CLIENT_ASYNC_REQUEST_COUNT(); i++) {
        helloClient.sayHello();
        helloClient.sayHelloAgain();
    }
    helloClient.waitForComplete();

    AsyncGoodbyeClient goodbyeClient(grpc::CreateChannel(serverAddress, grpc::InsecureChannelCredentials()));
    goodbyeClient.startThreadPool();
    for (int i = 0; i < CLIENT_CONFIG.GET_GRPC_CLIENT_ASYNC_REQUEST_COUNT(); i++) {
        goodbyeClient.sayGoodbye();
        goodbyeClient.sayGoodbyeAgain();
    }
    goodbyeClient.waitForComplete();
}

int main()
{
    InitLogger();
    SetLogLevel(CLIENT_CONFIG.GET_LOG_LEVEL());

    std::string serverAddress = std::string("0.0.0.0:") + std::to_string(CLIENT_CONFIG.GET_GRPC_SERVER_PORT());
    if (CLIENT_CONFIG.GET_GRPC_CLIENT_ASYNC()) {
        UseAsyncClient(serverAddress);
    }
    else {
        UseSyncClient(serverAddress);
    }

    return 0;
}

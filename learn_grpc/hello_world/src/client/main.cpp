#include "logger.h"
#include "config/client_config.h"
#include "hello_client.h"
#include "goodbye_client.h"
#include "chat_client.h"
#include <grpcpp/grpcpp.h>

int main()
{
    InitLogger();
    SetLogLevel(CLIENT_CONFIG.GET_LOG_LEVEL());

    std::string serverAddress = std::string("0.0.0.0:") + std::to_string(CLIENT_CONFIG.GET_GRPC_SERVER_PORT());

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

    return 0;
}

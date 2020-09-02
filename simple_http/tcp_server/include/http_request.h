#pragma once
#include <map>
#include <stdexcept>
#include "utils.h"
#include "log.h"
#include "request_handler.h"

class HttpRequest
{
    constexpr static char LINE_END[] = "\r\n";
    constexpr static size_t LINE_END_LEN = sizeof(LINE_END) - 1;
    constexpr static char HEADER_SEP = ':';
    constexpr static size_t HEADER_SEP_LEN = 1;
    constexpr static char CONTENT_LENGTH_HEADER[] = "Content-Length";

    std::string buf;
    size_t contentLength;

    std::string firstLine;
    std::map<std::string, std::string> headers;
    std::string messageBody;

    bool headersParseComplete;
    bool totalParseComplete;

    void SetFirstLine();
    void AddHeader(const std::string& s);
    void SetContentLength();
    void SetHeaders();
    void ParseMessageBody();
    void ParseRequest();

public:
    HttpRequest();
    void Append(const std::string& s);
    void Reset();
    void PrintRequest();
    bool AllReceived();
    std::string GetMessageBody();
};

class HttpRequestHandler : public RequestHandler
{
    HttpRequest httpRequest;
    std::string contentToWrite;

public:
    HttpRequestHandler(std::function<void(std::string)> write):
        RequestHandler(write)
    {}

    void Append(std::string s) override;
    bool CheckIntegrity() override;
    void ReadCompleteCallback(int errorCode) override;
    void WriteCompleteCallback(int errorCode) override;
    void Write(std::string s) override;
};

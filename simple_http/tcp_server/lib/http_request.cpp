#include "http_request.h"

constexpr char HttpRequest::LINE_END[];
constexpr size_t HttpRequest::LINE_END_LEN;
constexpr char HttpRequest::HEADER_SEP;
constexpr size_t HttpRequest::HEADER_SEP_LEN;
constexpr char HttpRequest::CONTENT_LENGTH_HEADER[];

void HttpRequest::SetFirstLine()
{
    if (!firstLine.empty()) {
        return;
    }

    size_t pos = buf.find(LINE_END);
    if (pos == std::string::npos) {
        return;
    }

    firstLine = buf.substr(0, pos);
    buf.erase(0, pos + LINE_END_LEN);
}

void HttpRequest::AddHeader(const std::string& s)
{
    size_t pos = s.find(HEADER_SEP);
    if (pos == std::string::npos) {
        throw std::runtime_error(__PRETTY_FUNCTION__);
    }
    std::string v = s.substr(pos + HEADER_SEP_LEN);
    Trim(v);
    headers[s.substr(0, pos)] = v;
}

void HttpRequest::SetContentLength()
{
    if (headers.count(CONTENT_LENGTH_HEADER) == 0) {
        contentLength = 0;
        return;
    }
    contentLength = std::stoi(headers[CONTENT_LENGTH_HEADER]);
}

void HttpRequest::SetHeaders()
{
    for (;;) {
        size_t pos = buf.find(LINE_END);
        if (pos == std::string::npos) {
            return;
        }

        if (pos == 0) {
            buf.erase(0, LINE_END_LEN);
            headersParseComplete = true;
            SetContentLength();
            return;
        }

        AddHeader(buf.substr(0, pos));
        buf.erase(0, pos + LINE_END_LEN);
    }
}

void HttpRequest::ParseMessageBody()
{
    if (!headersParseComplete) {
        return;
    }
    if (totalParseComplete) {
        return;
    }

    messageBody += buf;
    if (messageBody.size() >= contentLength) {
        totalParseComplete = true;
    }

    buf.clear();
}

void HttpRequest::ParseRequest()
{
    if (!headersParseComplete) {
        SetFirstLine();
        SetHeaders();
    }
    ParseMessageBody();
}

HttpRequest::HttpRequest():
    headersParseComplete(false),
    totalParseComplete(false),
    contentLength(0)
{}

void HttpRequest::Append(const std::string& s)
{
    if (totalParseComplete) {
        return;
    }
    buf += s;
    ParseRequest();
}

void HttpRequest::Reset()
{
    buf.clear();
}

void HttpRequest::PrintRequest()
{
    LogDebug("first line: ", firstLine);
    for (const auto x : headers) {
        LogDebug(x.first, ": ", x.second);
    }
    LogDebug("message body: ", messageBody);
}

bool HttpRequest::AllReceived()
{
    return totalParseComplete;
}

std::string HttpRequest::GetMessageBody()
{
    return messageBody;
}

void HttpRequestHandler::Append(std::string s)
{
    httpRequest.Append(s);
}

bool HttpRequestHandler::CheckIntegrity()
{
    return httpRequest.AllReceived();
}

void HttpRequestHandler::ReadCompleteCallback(int errorCode)
{
    if (errorCode != 0) {
        LogError("errorCode=", std::to_string(errorCode));
    }
    httpRequest.PrintRequest();
    std::string receivedMsg = httpRequest.GetMessageBody();
    std::string responseMsg = "hello\r\n";
    
    contentToWrite = "HTTP/1.1 200 OK\r\n";
    contentToWrite += "Context-Length: " +  std::to_string(responseMsg.size()) + "\r\n";
    contentToWrite += "\r\n";
    contentToWrite += responseMsg;

    Write(contentToWrite);
}

void HttpRequestHandler::Write(std::string s)
{
    RequestHandler::Write(s);
}

void HttpRequestHandler::WriteCompleteCallback(int errorCode)
{
    if (errorCode != 0) {
        LogError("errorCode=", std::to_string(errorCode));
    }
}

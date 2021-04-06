#pragma once

#include <vector>
#include <string.h>
#include "socket.h"

class ClientContext
{
	enum Config
	{
		BUFFER_SIZE = 1 << 12,
	};
	enum Option
	{
		OPTION_NONE = 0,
		OPTION_SEND = 1,
		OPTION_RECV = 2,
	};

	Option lastOption;

public:
	uint32_t id;
	SOCKET socket;

	OVERLAPPED overlapped;

	WSABUF wsaBuf;
	std::vector<char> buffer;
	uint32_t numberOfBytesToSend;
	uint32_t numberOfBytesSent;

	ClientContext(uint32_t id, SOCKET socket) :
		id(id),
		socket(socket),
		buffer(BUFFER_SIZE, 0),
		lastOption(OPTION_NONE),
		numberOfBytesSent(0),
		numberOfBytesToSend(0)
	{
		memset(&overlapped, 0, sizeof(overlapped));
	}

	~ClientContext()
	{
		CloseSocket(socket);
	}

	void setLastOptionRecv()
	{
		lastOption = OPTION_RECV;
	}

	void setLastOptionSend()
	{
		lastOption = OPTION_SEND;
	}

	bool isLastOptionRecv()
	{
		return lastOption == OPTION_RECV;
	}

	bool isLastOptionSend()
	{
		return lastOption == OPTION_SEND;
	}
};


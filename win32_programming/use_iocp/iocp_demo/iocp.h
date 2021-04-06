#pragma once

#include <thread>
#include <atomic>
#include <vector>
#include <memory>
#include <mutex>
#include "map"

class ClientContext;

class IOCP
{
	enum config
	{
		ACCEPT_WAIT_TIMEOUT_INTERVAL_MS = 500,
		INVALID_CLIENT_CONTEXT_ID = 1,
	};

	std::atomic<bool> started;
	std::atomic<bool> stopped;
	HANDLE iocp;
	SOCKET listenSocket;
	WSAEVENT acceptEvent;
	std::thread acceptThread;
	std::vector<std::shared_ptr<std::thread>> threadPool;
	std::mutex clientContextsLock;
	std::map<uint64_t, std::shared_ptr<ClientContext>> clientContexts;

	IOCP();
    void createAcceptThread();
	std::shared_ptr<ClientContext> createClientContext(SOCKET socket);
	void removeClientContext(uint32_t id);
	std::shared_ptr<ClientContext> getClientContext(uint32_t id);
	bool associateWithIOCP(std::shared_ptr<ClientContext> clientContext);
	void acceptConnection();
	void process();

public:
	void operator=(const IOCP&) = delete;
	IOCP(const IOCP&) = delete;
	static IOCP& getInstance();
	void start(int port, int concurrentNumber, int threadPoolSize);
	void stopThreadPool();
	void stop();
};


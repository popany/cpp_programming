#include <windows.h>
#include <stdexcept>
#include <string>
#include "socket.h"
#include "log.h"
#include "iocp.h"
#include "client_context.h"

// Create only an I/O completion port without associating it with a file handle.
static HANDLE CreateIOCP(int concurrentNumber)
{
    // Parameters
    // https://docs.microsoft.com/en-us/windows/win32/fileio/createiocompletionport
    //   `FileHandle` [in]
    //     If `INVALID_HANDLE_VALUE` is specified, the function creates an I/O completion port without associating it with a file handle.
    //     In this case, the `ExistingCompletionPort` parameter must be `NULL` and the `CompletionKey` parameter is ignored.
    //   `ExistingCompletionPort` [in, optional]
    //     If this parameter is `NULL`, the function creates a new I/O completion port.
    //   `CompletionKey` [in]
    //     The per-handle user-defined completion key
    //   `NumberOfConcurrentThreads` [in]
    //     The maximum number of threads that the operating system can allow to concurrently process I/O completion packets for the I/O completion port.
    //     This parameter is ignored if the `ExistingCompletionPort` parameter is not `NULL`.
    //     If this parameter is zero, the system allows as many concurrently running threads as there are processors in the system.
    HANDLE h = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, concurrentNumber);
    if (h == NULL) {
        DWORD code = GetLastError();
        throw std::runtime_error(std::string(__FUNCTION__) + " failed, code(" + std::to_string(code) + ")");
    }
    return h;
}

static void CloseAcceptEvent(WSAEVENT acceptEvent)
{
	if (FALSE == WSACloseEvent(acceptEvent)) {
		int code = WSAGetLastError();
		LogError("WSACloseEvent failed, code(", code, ")");
	}
}

static WSAEVENT CreateAcceptEvent(SOCKET listenSocket)
{
    WSAEVENT acceptEvent = WSACreateEvent();
    if (acceptEvent == WSA_INVALID_EVENT) {
        int code = WSAGetLastError();
        throw std::runtime_error(std::string(__FUNCTION__) + ", WSACreateEvent failed, code(" + std::to_string(code) + ")");
    }

    // WSAEventSelect
    // https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-wsaeventselect
    // Parameters
    //   `s`
    //     A descriptor identifying the socket.
    //   `hEventObject`
    //     A handle identifying the event object to be associated with the specified set of `FD_XXX` network events.
    //   `lNetworkEvents`
    //     A bitmask that specifies the combination of `FD_XXX` network events in which the application has interest.
    // Return value
    //   Having successfully recorded the occurrence of the network event (by setting the corresponding bit in the internal network event record) and
    //   signaled the associated event object, no further actions are taken for that network event until the application makes the function call that
    //   implicitly reenables the setting of that network event and signaling of the associated event object.
    //     - Network event `FD_ACCEPT` Re-enabling function - The accept, AcceptEx, or WSAAccept function unless the error code returned is 
    //                                                        `WSATRY_AGAIN` indicating that the condition function returned `CF_DEFER`.
    //   Any call to the reenabling routine, even one that fails, results in reenabling of recording and signaling for the relevant network event and
    //   event object.
    //   For `FD_READ`, `FD_OOB`, and `FD_ACCEPT` network events, network event recording and event object signaling are level-triggered. This means
    //   that if the reenabling routine is called and the relevant network condition is still valid after the call, the network event is recorded and
    //   the associated event object is set. This allows an application to be event-driven and not be concerned with the amount of data that arrives
    //   at any one time. Consider the following sequence:
    //     1. The transport provider receives 100 bytes of data on socket s and causes WS2_32.DLL to record the `FD_READ` network event and set the
    //        associated event object.
    //     2. The application issues `recv(s, buffptr, 50, 0)` to read 50 bytes.
    //     3. The transport provider causes WS2_32.DLL to record the `FD_READ` network event and sets the associated event object again since there is
    //        still data to be read.
    //   With these semantics, an application need not read all available data in response to an `FD_READ` network event - a single recv in response to
    //   each `FD_READ` network event is appropriate.
    //
    //   If a network event has already happened when the application calls `WSAEventSelect` or when the reenabling function is called, then a network
    //   event is recorded and the associated event object is set as appropriate. For example, consider the following sequence:
    //     1. An application calls `listen`.
    //     2. A connect request is received but not yet accepted.
    //     3. The application calls `WSAEventSelect` specifying that it is interested in the `FD_ACCEPT` network event for the socket. Due to the
    //        persistence of network events, Windows Sockets records the `FD_ACCEPT` network eventand sets the associated event object immediately.
    // Remarks
    //   The WSAEventSelect function automatically sets socket s to nonblocking mode, regardless of the value of lNetworkEvents.
    //
    //   The socket created when the accept function is called has the same properties as the listening socket used to accept it. Any `WSAEventSelect`
    //   association and network events selection set for the listening socket apply to the accepted socket. For example, if a listening socket has
    //   `WSAEventSelect` association of `hEventObject` with `FD_ACCEPT`, `FD_READ`, and `FD_WRITE`, then any socket accepted on that listening socket
    //   will also have `FD_ACCEPT`, `FD_READ`, and `FD_WRITE` network events associated with the same `hEventObject`. If a different `hEventObject` or
    //   network events are desired, the application should call `WSAEventSelect`, passing the accepted socket and the desired new information.
    if (SOCKET_ERROR == WSAEventSelect(listenSocket, acceptEvent, FD_ACCEPT)) {
        int code = WSAGetLastError();
        CloseAcceptEvent(acceptEvent);
        throw std::runtime_error(std::string(__FUNCTION__) + ", WSAEventSelect() failed, code(" + std::to_string(code) + ")");
    }
    return acceptEvent;
}

static void CloseIOCP(HANDLE& h)
{
    if (0 == CloseHandle(h)) {
        int code = GetLastError();
        LogError("CloseHandle() failed, code(", code, ")");
    }
    h = NULL;
}

IOCP::IOCP() :
    started(false),
    stopped(false),
    iocp(NULL),
    listenSocket(INVALID_SOCKET),
    acceptEvent(WSA_INVALID_EVENT)
{}

std::shared_ptr<ClientContext> IOCP::createClientContext()
{
    static uint32_t id = 0;
    std::lock_guard<std::mutex> lock(clientContextsLock);
    while (clientContexts.count(id) || id == INVALID_CLIENT_CONTEXT_ID) {
        id++;
    }
    auto clientContext = std::make_shared<ClientContext>(id);
    clientContexts[id] = clientContext;
    return clientContext;
}

void IOCP::removeClientContext(uint32_t id)
{
    std::lock_guard<std::mutex> lock(clientContextsLock);
    if (!clientContexts.count(id)) {
        LogError("ClientContext id(", clientCount->id, ") not exist");
        return;
    }
    clientContexts.erase(id);
}

std::shared_ptr<ClientContext> IOCP::getClientContext(uint32_t id)
{
    std::lock_guard<std::mutex> lock(clientContextsLock);
    if (!clientContexts.count(id)) {
        LogError("ClientContext id(", clientCount->id, ") not exist");
        return nullptr;
    }
    return clientContexts[id];
}

bool IOCP::associateWithIOCP(std::shared_ptr<ClientContext> clientContext)
{
    // CreateIoCompletionPort
    // Parameters
    //   `FileHandle` [in]
    //     The handle must be to an object that supports overlapped I/O.
    //   `ExistingCompletionPort` [in, optional]
    //     A handle to an existing I/O completion port, the function associates it with the handle specified by the `FileHandle` parameter.
    //   `CompletionKey` [in]
    //     The per-handle user-defined completion key that is included in every I/O completion packet for the specified file handle.
    //   `NumberOfConcurrentThreads` [in]
    //     This parameter is ignored if the `ExistingCompletionPort` parameter is not `NULL`.
    HANDLE h = CreateIoCompletionPort((HANDLE)clientContext->socket, iocp, (DWORD)clientContext->id, 0);
    if (NULL == h) {
        DWORD code = GetLastError();
        LogError("CreateToCompletionPort failed, code(", code, ")");
        return false;
    }
    return true;
}

void IOCP::acceptConnection()
{
    sockaddr_in clientAddress;
    int len = sizeof(clientAddress);
    SOCKET socket = accept(listenSocket, (sockaddr*)&clientAddress, &len);
    if (socket == INVALID_SOCKET) {
        int code = WSAGetLastError();
        LogError("accept failed, code(", code, ")");
        return;
    }
    std::shared_ptr<ClientContext> clientContext = createClientContext();
    clientContext->socket = socket;
    if (associateWithIOCP(clientContext)) {
        DWORD numberOfbytes = 0;
        DWORD flags = 0;
        clientContext->wsaBuf.buf = &(clientContext->recvBuffer[0]);
        clientContext->wsaBuf.len = clientContext->recvBuffer.size();
        //---------------------------------------------------------------------------------------------------------------------------------------------------
        // WSARecv
        // https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-wsarecv
        // Parameters
        //   `s`
        //     A descriptor identifying a connected socket.
        //   `lpBuffers`
        //     A pointer to an array of `WSABUF` structures. Each `WSABUF` structure contains a pointer to a buffer and the length, in bytes, of the buffer.
        //   `dwBufferCount`
        //     The number of `WSABUF` structures in the `lpBuffers` array.
        //   `lpNumberOfBytesRecvd`
        //     A pointer to the number, in bytes, of data received by this call if the receive operation completes immediately.
        //   `lpFlags`
        //     A pointer to flags used to modify the behavior of the `WSARecv` function call.
        //   `lpOverlapped`
        //     A pointer to a `WSAOVERLAPPED` structure
        //   `lpCompletionRoutine`
        //     A pointer to the completion routine called when the receive operation has been completed
        //
        // Return value
        //   If no error occurs and the receive operation has completed immediately, `WSARecv` returns zero. In this case, the completion routine will have
        //   already been scheduled to be called once the calling thread is in the alertable state. Otherwise, a value of `SOCKET_ERROR` is returned, and a
        //   specific error code can be retrieved by calling `WSAGetLastError`. The error code `WSA_IO_PENDING` indicates that the overlapped operation has
        //   been successfully initiated and that completion will be indicated at a later time. Any other error code indicates that the overlapped operation
        //   was not successfully initiated and no completion indication will occur.
        //
        // Remarks
        //   For overlapped sockets, `WSARecv` is used to post one or more buffers into which incoming data will be placed as it becomes available, after
        //   which the application specified completion indication (invocation of the completion routine or setting of an event object) occurs. If the
        //   operation does not complete immediately, the final completion status is retrieved through the completion routine or `WSAGetOverlappedResult`.
        //
        //   For byte stream-style sockets (for example, type `SOCK_STREAM`), incoming data is placed into the buffers until the buffers are filled, the
        //   connection is closed, or the internally buffered data is exhausted. Regardless of whether or not the incoming data fills all the buffers, the
        //   completion indication occurs for overlapped sockets.
        //
        // Overlapped Socket I/O
        //   If an overlapped operation completes immediately, `WSARecv` returns a value of zero and the `lpNumberOfBytesRecvd` parameter is updated with
        //   the number of bytes received and the flag bits indicated by the `lpFlags` parameter are also updated. If the overlapped operation is successfully
        //   initiated and will complete later, `WSARecv` returns `SOCKET_ERROR` and indicates error code `WSA_IO_PENDING`. In this case,
        //   `lpNumberOfBytesRecvd` and `lpFlags` are not updated. When the overlapped operation completes, the amount of data transferred is indicated either
        //   through the `cbTransferred` parameter in the completion routine (if specified), or through the `lpcbTransfer` parameter in
        //   `WSAGetOverlappedResult`. Flag values are obtained by examining the `lpdwFlags` parameter of `WSAGetOverlappedResult`. 
        //
        //   The `lpOverlapped` parameter must be valid for the duration of the overlapped operation. If multiple I/O operations are simultaneously
        //   outstanding, each must reference a separate `WSAOVERLAPPED` structure.
        //
        //   If the `lpCompletionRoutine` parameter is NULL, the `hEvent` parameter of `lpOverlapped` is signaled when the overlapped operation completes if
        //   it contains a valid event object handle. An application can use `WSAWaitForMultipleEvents` or `WSAGetOverlappedResult` to wait or poll on the
        //   event object.
        //   
        //   The completion routine follows the same rules as stipulated for Windows file I/O completion routines. The completion routine will not be invoked
        //   until the thread is in an alertable wait state such as can occur when the function `WSAWaitForMultipleEvents` with the `fAlertable` parameter set
        //   to `TRUE` is invoked.
        //
        //   If you are using I/O completion ports, be aware that the order of calls made to `WSARecv` is also the order in which the buffers are populated.
        //   `WSARecv` should not be called on the same socket simultaneously from different threads, because it can result in an unpredictable buffer order.
        //---------------------------------------------------------------------------------------------------------------------------------------------------
        // Post initial Recv.
		// This is a right place to post a initial Recv.
		// Posting a initial Recv in WorkerThread will create scalability issues.
        int r = WSARecv(clientContext->socket, &(clientContext->wsaBuf), 1, &numberOfbytes, &flags, &(clientContext->overlapped), NULL);
        if (r == SOCKET_ERROR) {
            int code = WSAGetLastError();
            if (code != WSA_IO_PENDING) {
                LogError("WSARecv() failed, code(", code, ")");
                removeClientContext(clientContext->id);
                return;
            }
        }
        clientContext->setLastOptionRecv();
    }
}

void IOCP::createAcceptThread()
{
    acceptThread = std::thread([&]() {
        while (!stopped) {
            // WSAWaitForMultipleEvents
            // https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-wsawaitformultipleevents
            // Parameters
            //   `cEvents`
            //     The number of event object handles in the array pointed to by `lphEvents`.
            //   `lphEvents`
            //     A pointer to an array of event object handles.
            //   `fWaitAll`
            //     A value that specifies the wait type. If `TRUE`, the function returns when the state of all objects in the `lphEvents` array is
            //     signaled. If `FALSE`, the function returns when any of the event objects is signaled.
            //   `dwTimeout`
            //     The time-out interval, in milliseconds. `WSAWaitForMultipleEvents` returns if the time-out interval expires, even if conditions
            //     specified by the `fWaitAll` parameter are not satisfied. If the `dwTimeout` parameter is zero, `WSAWaitForMultipleEvents` tests
            //     the state of the specified event objectsand returns immediately. If `dwTimeout` is `WSA_INFINITE`, `WSAWaitForMultipleEvents` waits
            //     forever; that is, the time-out interval never expires.
            //   `fAlertable`
            //     A value that specifies whether the thread is placed in an alertable wait state so the system can execute I/O completion routines.
            //     If `TRUE`, the thread is placed in an alertable wait state and `WSAWaitForMultipleEvents` can return when the system executes an I/O
            //     completion routine. In this case, `WSA_WAIT_IO_COMPLETION` is returned and the event that was being waited on is not signaled yet.
            //     The application must call the `WSAWaitForMultipleEvents` function again. If `FALSE`, the thread is not placed in an alertable wait
            //     state and I/O completion routines are not executed.
            DWORD r = WSAWaitForMultipleEvents(1, &acceptEvent, FALSE, ACCEPT_WAIT_TIMEOUT_INTERVAL_MS, FALSE);
            if (WSA_WAIT_TIMEOUT == r) {
                continue;
            }
            if (WSA_WAIT_FAILED == r) {
                int code = WSAGetLastError();
                LogError("WSAWaitForMultipleEvents() failed, code(", code, ")");
                continue;
            }
			WSANETWORKEVENTS wsaEvents;
            // WSAEnumNetworkEvents
            // https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-wsaenumnetworkevents
            // Parameters
            //   `s`
            //     A descriptor identifying the socket.
            //   `hEventObject`
            //     An optional handle identifying an associated event object to be reset.
            //   `lpNetworkEvents`
            //     A pointer to a `WSANETWORKEVENTS` structure that is filled with a record of network events that occurredand any associated error
            //     codes.
            if (SOCKET_ERROR == WSAEnumNetworkEvents(listenSocket, acceptEvent, &wsaEvents)) {
                int code = WSAGetLastError();
                LogError("WSAEnumNetworkEvents() failed, code(", code, ")");
                continue;
            }
            if (0 == wsaEvents.iErrorCode[FD_ACCEPT_BIT]) {
                LogError("FD_ACCEPT failed, code(", code, ")");
                continue;
            }
			if (wsaEvents.lNetworkEvents & FD_ACCEPT) {
				acceptConnection();
			}
        }
    });
}

void IOCP::process()
{
    DWORD numberOfBytes = 0;
    DWORD key = 0;
    OVERLAPPED *overlapped = NULL;
    // GetQueuedCompletionStatus
    // https://docs.microsoft.com/en-us/windows/win32/api/ioapiset/nf-ioapiset-getqueuedcompletionstatus
    // Parameters
    //   `CompletionPort`
    //     A handle to the completion port. To create a completion port, use the `CreateIoCompletionPort` function.
    //   `lpNumberOfBytesTransferred`
    //     A pointer to a variable that receives the number of bytes transferred in a completed I/O operation.
    //   `lpCompletionKey`
    //     A pointer to a variable that receives the completion key value associated with the file handle whose I/O operation has completed. A completion
    //     key is a per-file key that is specified in a call to `CreateIoCompletionPort`.
    //   `lpOverlapped`
    //     A pointer to a variable that receives the address of the `OVERLAPPED` structure that was specified when the completed I/O operation was started.
    //   `dwMilliseconds`
    //     The number of milliseconds that the caller is willing to wait for a completion packet to appear at the completion port. If a completion packet does
    //     not appear within the specified time, the function times out, returns `FALSE`, and sets* lpOverlapped to NULL.
    //
    //     If `dwMilliseconds` is `INFINITE`, the function will never time out. If `dwMilliseconds` is zeroand there is no I/O operation to dequeue, the
    //     function will time out immediately.
    //
    // Remarks
    //   This function associates a thread with the specified completion port. A thread can be associated with at most one completion port.
    //   
    //   If the `GetQueuedCompletionStatus` function succeeds, it dequeued a completion packet for a successful I/O operation from the completion port and has
    //   stored information in the variables pointed to by the following parameters: `lpNumberOfBytes`, `lpCompletionKey`, and `lpOverlapped`.
    BOOL r = GetQueuedCompletionStatus(iocp, &numberOfBytes, &key, &overlapped, INFINITE);
    if (r == FALSE) {
        DWORD code = GetLastError();
        LogError("GetQueuedCompletionStatus() failed, code(", code, ")");
        return;
    }
    if (key == INVALID_CLIENT_CONTEXT_ID) {
        return;
    }
    if (numberOfBytes == 0) {
        removeClientContext(key);
        return;
    }
    auto clientContext = getClientContext(key);
    if (clientContext == nullptr) {
        return;
    }
    if (clientContext->isLastOptionRecv()) {
        ;
    }
    else if (clientContext->isLastOptionSend()) {
        ;
    }
}

// Parameters
//   concurrentNumber & threadPoolSize
//     https://docs.microsoft.com/en-us/windows/win32/fileio/i-o-completion-ports
//     The system also allows a thread waiting in `GetQueuedCompletionStatus` to process a completion packet if another running thread associated
//     with the same I/O completion port enters a wait state for other reasons, for example the `SuspendThread` function. When the thread in the
//     wait state begins running again, there may be a brief period when the number of active threads exceeds the concurrency value. However, the
//     system quickly reduces this number by not allowing any new active threads until the number of active threads falls below the concurrency
//     value. This is one reason to have your application create more threads in its thread pool than the concurrency value.
void IOCP::start(int port, int concurrentNumber, int threadPoolSize)
{
    bool expected = false;
    if (!started.compare_exchange_strong(expected, true)) {
        throw std::runtime_error(std::string(__FUNCTION__) + ", already inited");
    }

    iocp = CreateIOCP(concurrentNumber);
    listenSocket = CreateListenSocket(port);
    acceptEvent = CreateAcceptEvent(listenSocket);
    createAcceptThread();

    for (int i = 0; i < threadPoolSize; i++) {
        threadPool.push_back(std::make_shared<std::thread>([&]() {
            while (!stopped) {
                process();
            }
        }));
    }
}

void IOCP::stop()
{
    stopped = true;
    acceptThread.join();
    for (auto t : threadPool) {
        t->join();
    }
    CloseIOCP(iocp);
    CloseSocket(listenSocket);
    CloseAcceptEvent(acceptEvent);
}



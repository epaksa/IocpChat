#include "stdafx.h"
#include "NetworkController.h"
#include "Logger.h"
#include "Statistics.h"

void NetworkController::InitWinsock()
{
	WSADATA wsaData;
	int resultCode = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (resultCode != 0) {
		PrintLog(eLogLevel::Error, "NetworkController::InitWinsock() -> %d", resultCode);
		WSACleanup();
	}
	else {
		PrintLog(eLogLevel::Info, "NetworkController::InitWinsock()");
	}
}

void NetworkController::InitIocpController() {
	mIocpController.CreateIocpHandle();
	mIocpController.CreateThreadPool(this);
}

void NetworkController::PostReceive(const SocketContextPointer clientContext)
{
	assert(clientContext != NULL && "clientContext is NULL.");

	DWORD sizeReceived = 0;
	DWORD flags = 0;

	ZeroMemory(&(clientContext->ReceiveContext->Overlapped), sizeof(clientContext->ReceiveContext->Overlapped));

	int resultCode = WSARecv(clientContext->Socket, &(clientContext->ReceiveContext->Wsabuf), 1,
		&sizeReceived, &flags, &(clientContext->ReceiveContext->Overlapped), NULL);

	if (resultCode == SOCKET_ERROR) {
		int errorCode = WSAGetLastError();
		if (errorCode != WSA_IO_PENDING) {
			PrintLog(eLogLevel::Error, "WSARecv() - client(%d), Error code(%d)", clientContext->Socket, errorCode);
		}
	}
}

void NetworkController::PostSend(const SocketContextPointer clientContext, const char* msg) {
	assert(clientContext != NULL && "clientContext is NULL.");

	DWORD byteSent = 0;

	ZeroMemory(&(clientContext->SendContext->Overlapped), sizeof(clientContext->SendContext->Overlapped));
	CopyMemory(clientContext->SendContext->Wsabuf.buf, msg, strlen(msg) + 1);
	clientContext->SendContext->Wsabuf.len = strlen(msg) + 1;
	
	int resultCode = WSASend(clientContext->Socket, &(clientContext->SendContext->Wsabuf), 1,
		&byteSent, 0, &(clientContext->SendContext->Overlapped), NULL);

	if (resultCode == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			PrintLog(eLogLevel::Error, "WSASend() - %d", errorCode);
		}
	}
}

void NetworkController::ProceedReceive(const SocketContextPointer clientContext, const DWORD receivedBytes)
{
	assert(clientContext != NULL && "clientContext is NULL.");

	PrintLog(eLogLevel::Info, "packet received from client(%d) / thread id : %d / msg : %s", 
		clientContext->Socket, GetCurrentThreadId(), clientContext->ReceiveContext->Wsabuf.buf);

	Statistics* stats = Statistics::GetInstance();
	stats->AddReceiveBytes(receivedBytes);

	PostSend(clientContext, clientContext->ReceiveContext->Wsabuf.buf);
}

void NetworkController::ProceedSend(const SocketContextPointer clientContext)
{
	assert(clientContext != NULL && "clientContext is NULL.");

	PrintLog(eLogLevel::Info, "packet sent to client(%d) / thread id : %d / msg : %s",
		clientContext->Socket, GetCurrentThreadId(), clientContext->SendContext->Wsabuf.buf);

	Statistics* stats = Statistics::GetInstance();
	stats->AddSendBytes(clientContext->SendContext->Wsabuf.len);

	// 요청처리결과 전송 후 다음 요청을 기다림
	PostReceive(clientContext);
}

SocketContextPointer NetworkController::CreateSocketContext(const SOCKET clientSocket) {
	assert(clientSocket != NULL && "clientSocket is NULL.");

	SocketContextPointer contextPointer = new SocketContext;
	IoContextPointer receivePointer = new IoContext;
	IoContextPointer sendPointer = new IoContext;

	contextPointer->Socket = clientSocket;
	contextPointer->ReceiveContext = receivePointer;
	contextPointer->SendContext = sendPointer;

	contextPointer->ReceiveContext->Wsabuf.buf = new char[SIZE_SOCKET_BUFFER];
	contextPointer->SendContext->Wsabuf.buf = new char[SIZE_SOCKET_BUFFER];
	contextPointer->ReceiveContext->Wsabuf.len = SIZE_SOCKET_BUFFER;
	contextPointer->SendContext->Wsabuf.len = SIZE_SOCKET_BUFFER;

	ZeroMemory(contextPointer->ReceiveContext->Wsabuf.buf, sizeof(char) * SIZE_SOCKET_BUFFER);
	ZeroMemory(contextPointer->SendContext->Wsabuf.buf, sizeof(char) * SIZE_SOCKET_BUFFER);

	return contextPointer;
}

void NetworkController::DeleteSocketContext(const SocketContextPointer contextPointer) {
	assert(contextPointer != NULL && "contextPointer is NULL.");
	assert(contextPointer->Socket != INVALID_SOCKET && "contextPointer->Socket is INVALID_SOCKET.");
	assert(contextPointer->ReceiveContext != NULL && "contextPointer->ReceiveContext is NULL.");
	assert(contextPointer->SendContext != NULL && "contextPointer->SendContext is NULL.");

	closesocket(contextPointer->Socket);

	delete contextPointer->ReceiveContext->Wsabuf.buf;
	delete contextPointer->SendContext->Wsabuf.buf;
	delete contextPointer->ReceiveContext;
	delete contextPointer->SendContext;
	delete contextPointer;
}

void NetworkController::CreateListenSocket() {
	mListenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (mListenSocket == INVALID_SOCKET) {
		PrintLog(eLogLevel::Error, "NetworkController::CreateListenSocket() -> %d", WSAGetLastError());
		WSACleanup();
	}
	else {
		PrintLog(eLogLevel::Info, "NetworkController::CreateListenSocket()");
	}
}

void NetworkController::Bind(const u_short port) {
	assert(port > -1 && "port is negative.");

	// 주소, port 설정
	SOCKADDR_IN sockAddr;
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	sockAddr.sin_port = htons(port);

	if (bind(mListenSocket, (LPSOCKADDR)&sockAddr, sizeof(sockAddr)) == SOCKET_ERROR) {
		PrintLog(eLogLevel::Error, "NetworkController::Bind() -> %d", WSAGetLastError());
		closesocket(mListenSocket);
		WSACleanup();
	}
	else {
		PrintLog(eLogLevel::Info, "NetworkController::Bind()");
	}
}

void NetworkController::StartListening() {
	if (listen(mListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		PrintLog(eLogLevel::Error, "NetworkController::StartListening() -> %d", WSAGetLastError());
		closesocket(mListenSocket);
		WSACleanup();
	}
	else {
		PrintLog(eLogLevel::Info, "NetworkController::StartListening()");
	}
}

void NetworkController::WaitForAccept() {
	AcceptThreadParam* params = new AcceptThreadParam;
	params->ListenSocket = mListenSocket;
	params->IocpController = &mIocpController;
	params->NetworkController = this;

	mAcceptThread = (HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)AcceptThread::StartThread, params, 0, 0);
}

void NetworkController::CloseServer()
{
	assert(mListenSocket != INVALID_SOCKET && "mListenSocket is INVALID_SOCKET.");

	mIocpController.TerminateThreads();
	DeleteAllClientContext();
	closesocket(mListenSocket);
	mListenSocket = INVALID_SOCKET;
	CloseHandle(mAcceptThread);
	WSACleanup();

	PrintLog(eLogLevel::Info, "NetworkController::CloseServer()");
}

void NetworkController::AddClientContextToSet(const SocketContextPointer clientContext)
{
	assert(clientContext != NULL && "clientContext is NULL.");
	mSetSocketContext.insert(clientContext);
}

void NetworkController::DeleteClientContextInSet(const SocketContextPointer key)
{
	assert(key != NULL && "key is NULL.");
	mSetSocketContext.erase(key);
}

void NetworkController::DeleteAllClientContext()
{
	Statistics* stats = Statistics::GetInstance();

	set<SocketContextPointer>::iterator socketIter = mSetSocketContext.begin();
	const set<SocketContextPointer>::iterator endIter = mSetSocketContext.end();

	while (socketIter != endIter) {
		SocketContextPointer clientContext = *socketIter;
		DeleteSocketContext(clientContext);
		stats->DecreaseConnection();
		socketIter++;
	}

	mSetSocketContext.clear();
}

void NetworkController::AcceptThread::StartThread(AcceptThreadParam* const params)
{
	Statistics* stats = Statistics::GetInstance();
	stats->IncreaseThreadNum();

	SOCKET listenSocket = params->ListenSocket;
	NetworkController* networkController = params->NetworkController;
	IocpController* iocpController = params->IocpController;

	SOCKADDR_IN clientSockAddr;
	int sizeSockAddr = sizeof(SOCKADDR_IN);

	PrintLog(eLogLevel::Info, "Starting accept()");

	while (true) {
		stats->SetThreadBlocked();
		SOCKET clientSocket = accept(listenSocket, (LPSOCKADDR)&clientSockAddr, &sizeSockAddr);
		stats->SetThreadActive();

		if (clientSocket == INVALID_SOCKET) {
			int errorCode = WSAGetLastError();

			// listen socket이 close
			if (errorCode == WSAEINTR) {
				PrintLog(eLogLevel::Error, "closed listen socket. (WSAEINTR)");
				break;
			}

			if (errorCode == WSAENOTSOCK) {
				PrintLog(eLogLevel::Error, "closed listen socket. (WSAENOTSOCK)");
				break;
			}

			PrintLog(eLogLevel::Error, "Accepting Client Error - %d", errorCode);
		}

		char ipBuf[64];
		inet_ntop(AF_INET, &(clientSockAddr.sin_addr), ipBuf, sizeof(ipBuf));

		PrintLog(eLogLevel::Info, "accepted!! - client(%d) / IP : %s / Port : %d", clientSocket, ipBuf, ntohs(clientSockAddr.sin_port));

		SocketContextPointer clientContext = networkController->CreateSocketContext(clientSocket);
		networkController->AddClientContextToSet(clientContext);
		iocpController->Associate((HANDLE)clientSocket, reinterpret_cast<ULONG_PTR>(clientContext));
		stats->IncreaseConnection();

		networkController->PostReceive(clientContext);
	}

	stats->DecreaseThreadNum();
	delete params;
}

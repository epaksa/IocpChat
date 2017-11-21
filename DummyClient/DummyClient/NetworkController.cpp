#include "stdafx.h"
#include "NetworkController.h"
#include "Logger.h"

void NetworkController::Init() {
	WSADATA wsaData;
	int resultCode = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (resultCode != 0) {
		PrintLog(eLogLevel::Error, "NetworkController::Init() -> %d", resultCode);
		WSACleanup();
	}
	else {
		PrintLog(eLogLevel::Info, "NetworkController::Init()");
	}
}

SOCKET NetworkController::CreateSocket() {
	SOCKET socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (socket == INVALID_SOCKET) {
		PrintLog(eLogLevel::Error, "NetworkController::CreateSocket() -> %d", WSAGetLastError());
		WSACleanup();
	}
	else {
		PrintLog(eLogLevel::Info, "NetworkController::CreateSocket()");
	}

	return socket;
}

SocketContextPointer NetworkController::Connect(const SOCKET socket, const char* ip, const u_short port, const int clientId) {
	assert(socket != INVALID_SOCKET && "socket is INVALID_SOCKET.");
	assert(ip != NULL && "invalid IP address.");

	SOCKADDR_IN sockAddr;
	sockAddr.sin_family = AF_INET;
	inet_pton(AF_INET, ip, (PVOID)&sockAddr.sin_addr.s_addr);
	sockAddr.sin_port = htons(port);

	if (connect(socket, (LPSOCKADDR)&sockAddr, sizeof(sockAddr)) == SOCKET_ERROR) {
		PrintLog(eLogLevel::Error, "NetworkController::Connect() -> %d", WSAGetLastError());
		closesocket(socket);
		WSACleanup();
		return nullptr;
	}
	else {
		PrintLog(eLogLevel::Info, "NetworkController::Connect() - connected !!");
		SocketContextPointer socketContext = CreateSocketContext(socket, clientId);
		mIocpController.Associate((HANDLE)socket, reinterpret_cast<ULONG_PTR>(socketContext));
		return socketContext;
	}
}

void NetworkController::ProceedIoRequest()
{
	mIocpController.GetIoFromIocpQueue(this);
}

void NetworkController::InitIocpController() {
	mIocpController.CreateIocpHandle();
}

void NetworkController::PostReceive(const SocketContextPointer socketContext)
{
	assert(socketContext != NULL && "socketContext is NULL.");

	DWORD sizeReceived = 0;
	DWORD flags = 0;

	ZeroMemory(&(socketContext->ReceiveContext->Overlapped), sizeof(socketContext->ReceiveContext->Overlapped));

	int resultCode = WSARecv(socketContext->Socket, &(socketContext->ReceiveContext->Wsabuf), 1,
		&sizeReceived, &flags, &(socketContext->ReceiveContext->Overlapped), NULL);

	if (resultCode == SOCKET_ERROR) {
		int errorCode = WSAGetLastError();
		if (errorCode != WSA_IO_PENDING) {
			PrintLog(eLogLevel::Error, "WSARecv() - client(%d), Error code(%d)", socketContext->Socket, errorCode);
		}
	}
}

void NetworkController::PostSend(const SocketContextPointer socketContext, const char* msg) {
	DWORD byteSent = 0;

	ZeroMemory(&(socketContext->SendContext->Overlapped), sizeof(socketContext->SendContext->Overlapped));
	strcpy_s(socketContext->SendContext->Wsabuf.buf, strlen(msg) + 1, msg);
	socketContext->SendContext->Wsabuf.len = (ULONG)strlen(socketContext->SendContext->Wsabuf.buf) + 1;

	int resultCode = WSASend(socketContext->Socket, &(socketContext->SendContext->Wsabuf), 1,
		&byteSent, 0, &(socketContext->SendContext->Overlapped), NULL);

	if (resultCode == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			PrintLog(eLogLevel::Error, "WSASend() - %d", errorCode);
		}
	}
}

void NetworkController::ProceedReceive(const SocketContextPointer socketContext, const DWORD receivedBytes)
{
	PrintLog(eLogLevel::Info, "packet received from server(%d) / thread(%d) / id : %d / msg : %s",
		socketContext->Socket, GetCurrentThreadId(), socketContext->ID, socketContext->ReceiveContext->Wsabuf.buf);
}

void NetworkController::ProceedSend(const SocketContextPointer socketContext)
{
	PrintLog(eLogLevel::Info, "packet sent to server(%d) / thread(%d) / id : %d / msg : %s", 
		socketContext->Socket, GetCurrentThreadId(), socketContext->ID, socketContext->SendContext->Wsabuf.buf);
}

SocketContextPointer NetworkController::CreateSocketContext(const SOCKET socket, const int clientId) {
	assert(socket != NULL && "socket is NULL.");

	SocketContextPointer socketContext = new SocketContext;
	IoContextPointer receivePointer = new IoContext;
	IoContextPointer sendPointer = new IoContext;

	socketContext->Socket = socket;
	socketContext->ID = clientId;
	socketContext->ReceiveContext = receivePointer;
	socketContext->SendContext = sendPointer;

	socketContext->ReceiveContext->Wsabuf.buf = new char[SIZE_SOCKET_BUFFER];
	socketContext->SendContext->Wsabuf.buf = new char[SIZE_SOCKET_BUFFER];
	socketContext->ReceiveContext->Wsabuf.len = SIZE_SOCKET_BUFFER;
	socketContext->SendContext->Wsabuf.len = SIZE_SOCKET_BUFFER;

	ZeroMemory(socketContext->ReceiveContext->Wsabuf.buf, sizeof(char) * SIZE_SOCKET_BUFFER);
	ZeroMemory(socketContext->SendContext->Wsabuf.buf, sizeof(char) * SIZE_SOCKET_BUFFER);

	return socketContext;
}

void NetworkController::DeleteSocketContext(const SocketContextPointer socketContext) {
	assert(socketContext != NULL && "socketContext is NULL.");
	assert(socketContext->Socket != INVALID_SOCKET && "socketContext->Socket is INVALID_SOCKET.");
	assert(socketContext->ReceiveContext != NULL && "socketContext->ReceiveContext is NULL.");
	assert(socketContext->SendContext != NULL && "socketContext->SendContext is NULL.");

	closesocket(socketContext->Socket);

	delete socketContext->ReceiveContext->Wsabuf.buf;
	delete socketContext->SendContext->Wsabuf.buf;
	delete socketContext->ReceiveContext;
	delete socketContext->SendContext;
	delete socketContext;
}

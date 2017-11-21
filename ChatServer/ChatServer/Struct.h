#pragma once

class NetworkController;
class IocpController;

typedef struct tagIoContext {
	WSAOVERLAPPED Overlapped;
	WSABUF Wsabuf;
} IoContext, *IoContextPointer;

typedef struct tagSocketContext {
	SOCKET Socket;
	IoContextPointer ReceiveContext;
	IoContextPointer SendContext;
} SocketContext, *SocketContextPointer;

typedef struct tagAcceptThread {
	SOCKET ListenSocket;
	NetworkController* NetworkController;
	IocpController* IocpController;
} AcceptThreadParam;

typedef struct tagIocpThreadParam {
	NetworkController* NetworkController;
	HANDLE IocpHandle;
} IocpThreadParam;
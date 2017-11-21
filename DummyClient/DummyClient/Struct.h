#pragma once

typedef struct tagIoContext {
	WSAOVERLAPPED Overlapped;
	WSABUF Wsabuf;
} IoContext, *IoContextPointer;

typedef struct tagSocketContext {
	SOCKET Socket;
	int ID;
	IoContextPointer ReceiveContext;
	IoContextPointer SendContext;
} SocketContext, *SocketContextPointer;

typedef struct tagThreadParam {
	int ClientCount;
	int ThreadId;
} ThreadParam;

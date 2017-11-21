#pragma once

#include "Constants.h"
#include "IocpController.h"
#include "Struct.h"

class NetworkController
{
public:
	// 일반 함수
	void Init();
	void InitIocpController();

	void PostReceive(const SocketContextPointer socketContext);
	void PostSend(const SocketContextPointer socketContext, const char* msg);
	void ProceedReceive(const SocketContextPointer socketContext, const DWORD receivedBytes);
	void ProceedSend(const SocketContextPointer socketContext);

	SocketContextPointer CreateSocketContext(const SOCKET socket, const int clientId);
	void DeleteSocketContext(const SocketContextPointer socketContext);

	// client 전용 function 
	SOCKET CreateSocket();
	SocketContextPointer Connect(const SOCKET socket, const char* ip, const u_short port, const int clientId);
	void ProceedIoRequest();

private:
	IocpController mIocpController;
};

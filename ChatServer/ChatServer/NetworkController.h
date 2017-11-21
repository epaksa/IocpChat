// NetworkController.h : ��� ��� Ŭ���� �� ���� ����ü
#pragma once

#include <set>
#include "Constants.h"
#include "IocpController.h"
#include "Struct.h"

class NetworkController
{
public:
	// �Ϲ� �Լ�
	void InitWinsock();
	void InitIocpController();

	void PostReceive(const SocketContextPointer clientContext);
	void PostSend(const SocketContextPointer clientContext, const char* msg);
	void ProceedReceive(const SocketContextPointer clientContext, const DWORD receivedBytes);
	void ProceedSend(const SocketContextPointer clientContext);

	SocketContextPointer CreateSocketContext(const SOCKET clientSocket);
	void DeleteSocketContext(const SocketContextPointer contextPointer);

	// server ���� function
	void CreateListenSocket();
	void Bind(const u_short port = DEFAULT_PORT);
	void StartListening();
	void WaitForAccept();
	void CloseServer();

	// client context map ����
	void AddClientContextToSet(const SocketContextPointer clientContext);
	void DeleteClientContextInSet(const SocketContextPointer key);
	void DeleteAllClientContext();

private:
	SOCKET mListenSocket;
	IocpController mIocpController;

	set<SocketContextPointer> mSetSocketContext;
	HANDLE mAcceptThread;

	class AcceptThread
	{
	public:
		static void __stdcall StartThread(AcceptThreadParam* const params);
	};
};

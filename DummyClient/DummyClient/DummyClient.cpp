#include "stdafx.h"
#include "Constants.h"
#include "Logger.h"
#include "NetworkController.h"
#include "Struct.h"

#define EXT
#include "CriticalSection.h"

DWORD __stdcall ExecuteClient(ThreadParam* param) {
	NetworkController networkController;
	networkController.InitIocpController();

	int clientPerThread = param->ClientCount;
	int threadId = param->ThreadId;
	const DWORD threadRealId = GetCurrentThreadId();

	SocketContextPointer* socketList = new SocketContextPointer[clientPerThread];
	// 서버에 연결
	for (int i = 0; i < clientPerThread; i++) {
		const SOCKET socket = networkController.CreateSocket();
		int clientId = (clientPerThread*(threadId - 1)) + (i + 1);
		socketList[i] = networkController.Connect(socket, IP_SERVER, PORT_SERVER, clientId);

		PrintLog(eLogLevel::Info, "#### Created Client %d (Thread ID : %d (%d) ) ####", clientId, threadId, threadRealId);
	}

	for (int msgCount = 0; msgCount < MESSAGE_COUNT; msgCount++) {
		for (int i = 0; i < clientPerThread; i++) {
			int clientId = (clientPerThread*(threadId - 1)) + (i + 1);
			string msg = "message from client ";
			msg += to_string(clientId);
			msg += " / count : ";
			msg += to_string(msgCount + 1);

			networkController.PostSend(socketList[i], msg.c_str());
			networkController.ProceedIoRequest();
			networkController.PostReceive(socketList[i]);
			networkController.ProceedIoRequest();
		}
	}

	for (int i = 0; i < clientPerThread; i++) {
		networkController.DeleteSocketContext(socketList[i]);
		int clientId = (clientPerThread*(threadId - 1)) + (i + 1);
		PrintLog(eLogLevel::Info, "#### End Client %d (Thread ID : %d (%d) ) ####", clientId, threadId, threadRealId);
	}

	delete socketList;
	delete param;
	return 0;
}

int main()
{
	int clientCount = 1;
	cout << "Enter client count (1~10000) : ";
	cin >> clientCount;

	if (clientCount > MAX_USER) {
		cout << "Error : tried to create too many client!!" << endl;
		return 0;
	}

	// 초기화
	InitializeCriticalSection(&lock);
	NetworkController networkController;
	networkController.Init();

	// client thread 생성
	HANDLE* threadList = new HANDLE[NUM_THREAD];

	for (int i = 0; i < NUM_THREAD; i++) {
		ThreadParam* threadParam = new ThreadParam;
		threadParam->ClientCount = clientCount / 8;
		threadParam->ThreadId = i + 1;

		threadList[i] = (HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)ExecuteClient, threadParam, 0, 0);
	}

	WaitForMultipleObjects(NUM_THREAD, threadList, TRUE, INFINITE);

	// 종료
	for (int i = 0; i < NUM_THREAD; i++) {
		CloseHandle(threadList[i]);
	}
	delete threadList;
	DeleteCriticalSection(&lock);
	WSACleanup();

	cout << "END" << endl;

	return 0;
}


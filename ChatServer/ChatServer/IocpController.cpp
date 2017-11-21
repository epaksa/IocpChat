#include "stdafx.h"
#include "IocpController.h"
#include "Logger.h"
#include "NetworkController.h"
#include "Statistics.h"

#define REQUEST_QUIT_THREAD (1)

IocpController::IocpController()
{
	mIocpHandle = NULL;
	mThreadList = NULL;
	mNumThread = 0;
}

IocpController::~IocpController()
{
	if (mThreadList != NULL) {
		for (int i = 0; i < mNumThread; i++) {
			CloseHandle(mThreadList[i]);
		}

		delete mThreadList;
	}
	mThreadList = NULL;
}

void IocpController::CreateIocpHandle() {
	mIocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	if (mIocpHandle == NULL) {
		PrintLog(eLogLevel::Error, "IocpController::CreateIocpPort() - %d", GetLastError());
	}
	else {
		PrintLog(eLogLevel::Info, "IocpController::CreateIocpPort()");
	}
}

void IocpController::CreateThreadPool(NetworkController* const networkController) {
	// 쓰레드 수 = 7 + console출력 thread + accept thread + main thread
	mNumThread = 7;
	mThreadList = new HANDLE[mNumThread];

	PrintLog(eLogLevel::Info, "Start CreateThreadPool()");

	for (int i = 0; i < mNumThread; i++) {
		IocpThreadParam* params = new IocpThreadParam;
		params->IocpHandle = mIocpHandle;
		params->NetworkController = networkController;
		mThreadList[i] = (HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)IocpThread::StartThread, params, 0, 0);
	}

	PrintLog(eLogLevel::Info, "Finished CreateThreadPool()");
}

void IocpController::Associate(const HANDLE deviceHandle, const ULONG_PTR completionKey)
{
	assert(deviceHandle != NULL && "deviceHandle is NULL.");
	assert(completionKey != NULL && "completionKey is NULL.");

	HANDLE returnedHandle = CreateIoCompletionPort(deviceHandle, mIocpHandle, completionKey, 0);

	if (returnedHandle != mIocpHandle) {
		PrintLog(eLogLevel::Error, "IocpController::Associate() - %d", GetLastError());
	}
	else {
		//PrintLog(INFO_LEVEL, "IocpController::Associate()");
	}
}

void IocpController::TerminateThreads()
{
	for (int i = 0; i < mNumThread; i++) {
		BOOL result = PostQueuedCompletionStatus(mIocpHandle, 0, REQUEST_QUIT_THREAD, NULL);
		if (!result) {
			PrintLog(eLogLevel::Error, "PostQueuedCompletionStatus - %d", GetLastError());
		}
	}

	PrintLog(eLogLevel::Info, "IocpController::TerminateThreads()");
}

void __stdcall IocpController::IocpThread::StartThread(IocpThreadParam* const params) {
	assert(params != NULL && "params is NULL.");
	assert(params->IocpHandle != NULL && "params->IocpHandle is NULL.");

	Statistics* stats = Statistics::GetInstance();
	stats->IncreaseThreadNum();

	DWORD transferredBytes = 0;
	SocketContextPointer completionKey = NULL;
	IoContextPointer ioType = NULL;
	NetworkController* networkController = params->NetworkController;
	const DWORD threadRealId = GetCurrentThreadId();

	PrintLog(eLogLevel::Info, "IocpThread::StartThread() - Started, thread(%d)", threadRealId);

	while (true) {
		stats->SetThreadBlocked();
		BOOL result = GetQueuedCompletionStatus(params->IocpHandle, &transferredBytes,
			reinterpret_cast<PULONG_PTR>(&completionKey),
			reinterpret_cast<LPOVERLAPPED*>(&ioType), INFINITE);
		stats->SetThreadActive();

		if (!result) {
			continue;
		}

		// thread종료하는 io요청인지 확인
		if ((int)completionKey == REQUEST_QUIT_THREAD) {
			break;
		}

		SocketContextPointer clientContext = completionKey;

		// client 종료시
		if (transferredBytes == 0) {
			networkController->DeleteClientContextInSet(clientContext);
			networkController->DeleteSocketContext(clientContext);
			stats->DecreaseConnection();
			continue;
		}

		// io type에 따른 처리동작 분기
		if (ioType == clientContext->ReceiveContext) {
			networkController->ProceedReceive(clientContext, transferredBytes);
		}
		else if (ioType == clientContext->SendContext) {
			networkController->ProceedSend(clientContext);
		}
		else {
			assert(false && "invalid ioType.");
		}
	}

	stats->DecreaseThreadNum();
	delete params;

	PrintLog(eLogLevel::Info, "IocpThread::StartThread() - Terminated, thread(%d)", threadRealId);
}
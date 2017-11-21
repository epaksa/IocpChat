#include "stdafx.h"
#include "IocpController.h"
#include "Logger.h"
#include "NetworkController.h"

void IocpController::CreateIocpHandle() {
	mIocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	if (mIocpHandle == NULL) {
		PrintLog(eLogLevel::Error, "IocpController::CreateIocpPort() - %d", GetLastError());
	}
	else {
		PrintLog(eLogLevel::Info, "IocpController::CreateIocpPort()");
	}
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
		PrintLog(eLogLevel::Info, "IocpController::Associate()");
	}
}

void IocpController::GetIoFromIocpQueue(NetworkController* const networkController)
{
	DWORD transferredBytes = 0;
	SocketContextPointer completionKey;
	IoContextPointer ioType = NULL;

	BOOL result = GetQueuedCompletionStatus(mIocpHandle, &transferredBytes,
		reinterpret_cast<PULONG_PTR>(&completionKey),
		reinterpret_cast<LPOVERLAPPED*>(&ioType), INFINITE);

	if (!result) {
		PrintLog(eLogLevel::Error, "GetQueuedCompletionStatus - Thread(%d), Error(%d)", GetCurrentThreadId(), GetLastError());
	}

	SocketContextPointer socketContext = completionKey;

	// 상대방 종료시
	if (transferredBytes == 0) {
		PrintLog(eLogLevel::Info, "Server(%d) disconnected - Thread (%d)", socketContext->Socket, GetCurrentThreadId());
		networkController->DeleteSocketContext(socketContext);
	}

	// io type에 따른 처리동작 분기
	if (ioType == socketContext->ReceiveContext) {
		networkController->ProceedReceive(socketContext, transferredBytes);
	}
	else if (ioType == socketContext->SendContext) {
		networkController->ProceedSend(socketContext);
	}
	else {
		assert(false && "invalid ioType.");
	}
}

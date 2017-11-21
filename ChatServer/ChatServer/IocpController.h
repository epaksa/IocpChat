// IocpController.h : IOCP 모듈 클래스 및 관련 구조체
#pragma once

typedef struct tagIocpThreadParam IocpThreadParam;
class NetworkController;
class DataController;

class IocpController
{
public:
	IocpController();
	~IocpController();

	void CreateIocpHandle();
	void CreateThreadPool(NetworkController* const networkController);
	void Associate(const HANDLE deviceHandle, const ULONG_PTR completionKey);
	void TerminateThreads();

private:
	HANDLE mIocpHandle;
	HANDLE* mThreadList;
	int mNumThread;

	// GetQueuedCompletionStatus()를 처리하는 thread가 있는 inner class
	class IocpThread
	{
	public:
		static void __stdcall StartThread(IocpThreadParam* const params);
	};
};


// IocpController.h : IOCP ��� Ŭ���� �� ���� ����ü
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

	// GetQueuedCompletionStatus()�� ó���ϴ� thread�� �ִ� inner class
	class IocpThread
	{
	public:
		static void __stdcall StartThread(IocpThreadParam* const params);
	};
};


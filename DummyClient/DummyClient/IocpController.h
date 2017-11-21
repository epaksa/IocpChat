#pragma once

class NetworkController;

class IocpController
{
public:
	void CreateIocpHandle();
	void Associate(const HANDLE deviceHandle, const ULONG_PTR completionKey);
	void GetIoFromIocpQueue(NetworkController* const networkController);

private:
	HANDLE mIocpHandle;
};

// Statistics.h : 네트워크 통계 모듈
#pragma once

class Statistics
{
private:
	Statistics();

public:
	~Statistics();

	static Statistics* GetInstance();

	void AddReceiveBytes(DWORD receiveBytes);
	void AddSendBytes(DWORD sendBytes);

	inline void IncreaseConnection() {
		InterlockedIncrement(&mNumConnection);
	}
	inline void DecreaseConnection() {
		InterlockedDecrement(&mNumConnection);
	}
	inline void SetThreadActive() {
		InterlockedIncrement(&mNumActiveThread);
		InterlockedDecrement(&mNumBlockedThread);
	}
	inline void SetThreadBlocked() {
		InterlockedIncrement(&mNumBlockedThread);
		InterlockedDecrement(&mNumActiveThread);
	}
	inline void IncreaseThreadNum() {
		InterlockedIncrement(&mNumActiveThread);
	}
	inline void DecreaseThreadNum() {
		InterlockedDecrement(&mNumActiveThread);
	}

	inline unsigned int GetConnectionNum() {
		return mNumConnection;
	}
	inline unsigned int GetActivethreadNum() {
		return mNumActiveThread;
	}
	inline unsigned int GetBlockedthreadNum() {
		return mNumBlockedThread;
	}
	inline unsigned long long GetTotalReceivedBytes() {
		return mTotalReceiveBytes;
	}
	inline unsigned long long GetTotalSentBytes() {
		return mTotalSendBytes;
	}

private:
	static Statistics* mInstance;

	volatile unsigned int mNumConnection;
	volatile unsigned int mNumActiveThread;
	volatile unsigned int mNumBlockedThread;

	unsigned long long mTotalReceiveBytes;
	unsigned long long mTotalSendBytes;

	CRITICAL_SECTION mCsReceive;
	CRITICAL_SECTION mCsSend;
};


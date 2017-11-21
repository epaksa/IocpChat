#include "stdafx.h"
#include "Statistics.h"
#include "Logger.h"

Statistics* Statistics::mInstance = NULL;

Statistics::Statistics()
{
	mNumConnection = 0;
	mNumActiveThread = 0;
	mNumBlockedThread = 0;

	mTotalReceiveBytes = 0;
	mTotalSendBytes = 0;

	InitializeCriticalSection(&mCsReceive);
	InitializeCriticalSection(&mCsSend);
}

Statistics::~Statistics()
{
	if (mInstance != NULL) {
		delete mInstance;
	}

	DeleteCriticalSection(&mCsReceive);
	DeleteCriticalSection(&mCsSend);
}

Statistics* Statistics::GetInstance()
{
	if (mInstance == NULL) {
		mInstance = new Statistics;
	}
	return mInstance;
}

void Statistics::AddReceiveBytes(DWORD receiveBytes)
{
	EnterCriticalSection(&mCsReceive);
	mTotalReceiveBytes += receiveBytes;
	LeaveCriticalSection(&mCsReceive);
}

void Statistics::AddSendBytes(DWORD sendBytes)
{
	EnterCriticalSection(&mCsSend);
	mTotalSendBytes += sendBytes;
	LeaveCriticalSection(&mCsSend);
}

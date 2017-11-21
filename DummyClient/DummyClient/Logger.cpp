#include "stdafx.h"
#include "Logger.h"
#include "CriticalSection.h"

void PrintLog(const eLogLevel level, const char *str, ...)
{
#ifndef PRINT_LOG
	return;
#endif

	assert(str != NULL && "str is NULL.");

	va_list argList;
	va_start(argList, str);

	char strWithLevel[1024];

	switch (level)
	{
	case eLogLevel::Info:
		strcpy_s(strWithLevel, "[Info] ");
		break;
	case eLogLevel::Error:
		strcpy_s(strWithLevel, "[Error] ");
		break;
	case eLogLevel::None:
		strcpy_s(strWithLevel, "");
		break;
	default:
		assert(0 && "invalid log level.");
		break;
	}

	strcat_s(strWithLevel, str);
	strcat_s(strWithLevel, "\n");

	EnterCriticalSection(&lock);

	vprintf(strWithLevel, argList);

	LeaveCriticalSection(&lock);

	va_end(argList);
}

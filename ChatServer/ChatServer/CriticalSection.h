// CriticalSection.h : ��Ƽ������ȯ�濡��
// ���α׷� ������ �����ϱ� ���� critical section ���
#pragma once

#include "stdafx.h"

#ifdef EXT
#define EXT
#else
#define EXT extern
#endif // EXT

EXT CRITICAL_SECTION lock;
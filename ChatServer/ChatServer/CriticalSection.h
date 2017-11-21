// CriticalSection.h : 멀티쓰레드환경에서
// 프로그램 동작을 제어하기 위해 critical section 사용
#pragma once

#include "stdafx.h"

#ifdef EXT
#define EXT
#else
#define EXT extern
#endif // EXT

EXT CRITICAL_SECTION lock;
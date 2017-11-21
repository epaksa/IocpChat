#pragma once

#include "stdafx.h"

#ifdef EXT
#define EXT
#else
#define EXT extern
#endif // EXT

EXT CRITICAL_SECTION lock;
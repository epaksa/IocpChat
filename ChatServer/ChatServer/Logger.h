// Logger.h : console에 출력할 log 모듈
#pragma once

#include "Enum.h"

void PrintLog(const eLogLevel level, const char *str, ...);
void OpenFile(const char* fileName);
void CloseFile();
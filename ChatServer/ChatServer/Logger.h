// Logger.h : console�� ����� log ���
#pragma once

#include "Enum.h"

void PrintLog(const eLogLevel level, const char *str, ...);
void OpenFile(const char* fileName);
void CloseFile();
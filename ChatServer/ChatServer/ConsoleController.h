#pragma once

#include "Constants.h"

class ConsoleController
{
public:
	static void __stdcall StartPrint(ConsoleController* const consoleController);

	void Init();
	void Quit();
	void PrintInitialState();
	void StartPrintThread();
	void UpdateInformation();
	void SetInvisibleCursor();
	void SetVisibleCursor();
	void SetScreenSize(const unsigned int cols, const unsigned int line);
	void ClearScreen();
	WORD getCurrentColor();

private:
	int PrintStr(const short x, const short y, const char* str, const WORD color = 0);
	void SetCursor(const short x, const short y);
	void SetColor(const WORD color);

private:
	HANDLE mStdHandle;
	HANDLE mPrintThread;
	clock_t mTimeStart;
};


#include "stdafx.h"
#include "ConsoleController.h"
#include "Statistics.h"
#include "Logger.h"

void __stdcall ConsoleController::StartPrint(ConsoleController* const consoleController)
{
	Statistics* stats = Statistics::GetInstance();
	stats->IncreaseThreadNum();

	clock_t timeStart, timeEnd;

	while (true) {
		timeStart = clock();
		consoleController->UpdateInformation();
		timeEnd = clock();

		Sleep(PRINT_INTERVAL_MS - (timeStart - timeEnd));	// 0.5초마다 화면 갱신
	}

	PrintLog(eLogLevel::Info, "ConsoleController::StartPrint() - Terminated");
}

void ConsoleController::Init()
{
	mStdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	mTimeStart = 0;
	SetInvisibleCursor();
}

void ConsoleController::Quit()
{
	ClearScreen();

	// print thread 종료
	DWORD exitcode = 0;
	GetExitCodeThread(mPrintThread, &exitcode);
	TerminateThread(mPrintThread, exitcode);
	CloseHandle(mPrintThread);

	SetVisibleCursor();

	Statistics* stats = Statistics::GetInstance();
	stats->DecreaseThreadNum();
}

void ConsoleController::PrintInitialState()
{
	SetScreenSize(DEFAULT_COLS, DEFAULT_LINE + 1);
	ClearScreen();

	for (int y = 0; y < DEFAULT_LINE; y++) {
		if (y == 0 || y == DEFAULT_LINE - 1) {	// 첫줄과 마지막줄 비우기
			cout << endl;
			continue;
		}

		for (int x = 0; x < DEFAULT_COLS; x++) {
			if (x == 0 || x == DEFAULT_COLS - XSIZE_EMPTY) {	// 양쪽 측면 2칸 비우기
				const int bytesWritten = PrintStr(x, y, CHAR_EMPTY);
				x += bytesWritten - 1;
			}
			else if (y == YPOS_INFO_TITLE && x == XPOS_INFO_TITLE) {
				const int bytesWritten = PrintStr(x, y, "== INFORMATION ==", COLOR_TEXT_WHITE);
				x += bytesWritten - 1;
			}
			else if (x == XPOS_INFO_START) {	// 정보 출력하는 부분
				if (y == YPOS_INFO_TITLE + 2) {
					const int bytesWritten = PrintStr(x, y, CHAR_INFO_PLAYTIME, COLOR_TEXT_WHITE);
					x += bytesWritten - 1;
				}
				else if (y == YPOS_INFO_TITLE + 4) {
					const int bytesWritten = PrintStr(x, y, CHAR_INFO_USER_COUNT, COLOR_TEXT_WHITE);
					x += bytesWritten - 1;
				}
				else if (y == YPOS_INFO_TITLE + 6) {
					const int bytesWritten = PrintStr(x, y, CHAR_INFO_THREAD_COUNT, COLOR_TEXT_WHITE);
					x += bytesWritten - 1;
				}
				else if (y == YPOS_INFO_TITLE + 8) {
					const int bytesWritten = PrintStr(x, y, CHAR_INFO_TOTAL_RECEIVED_BYTES, COLOR_TEXT_WHITE);
					x += bytesWritten - 1;
				}
				else if (y == YPOS_INFO_TITLE + 10) {
					const int bytesWritten = PrintStr(x, y, CHAR_INFO_TOTAL_SENT_BYTES, COLOR_TEXT_WHITE);
					x += bytesWritten - 1;
				}
				else if (y == YPOS_INFO_TITLE + 12) {
					const int bytesWritten = PrintStr(x, y, CHAR_INFO_RECEIVED_BPS, COLOR_TEXT_WHITE);
					x += bytesWritten - 1;
				}
				else if (y == YPOS_INFO_TITLE + 14) {
					const int bytesWritten = PrintStr(x, y, CHAR_INFO_SENT_BPS, COLOR_TEXT_WHITE);
					x += bytesWritten - 1;
				}
				else if (y == YPOS_INFO_TITLE + 16) {
					const int bytesWritten = PrintStr(x, y, CHAR_INFO_FILE, COLOR_TEXT_WHITE);
					x += bytesWritten - 1;
				}
				else {
					//PrintStr(x, y, "#", COLOR_TEXT_WHITE);
				}
			}
			else {
				//PrintStr(x, y, "#", COLOR_TEXT_WHITE);
			}
		}
	}
}

void ConsoleController::StartPrintThread()
{
	mTimeStart = clock();
	mPrintThread = (HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)StartPrint, this, 0, 0);
}

void ConsoleController::UpdateInformation()
{
	Statistics* stats = Statistics::GetInstance();
	const unsigned int numCurrentConnection = stats->GetConnectionNum();
	const unsigned int numActiveThread = stats->GetActivethreadNum();
	const unsigned int numBlockedThread = stats->GetBlockedthreadNum();
	const unsigned long long totalReceivedBytes = stats->GetTotalReceivedBytes();
	const unsigned long long totalSentBytes = stats->GetTotalSentBytes();

	// 실행 시간 측정
	const clock_t timeCurrent = clock();
	long execTimeTotalSec = (timeCurrent - mTimeStart) / 1000;	// 초단위 경과시간
	long execTimeMin = execTimeTotalSec / 60;	// 분단위 경과시간
	long execTimeSec = execTimeTotalSec % 60;	// 초부분만 잘라낸시간
	string strMin, strSec;

	if (execTimeMin < 10) {
		strMin = "0" + to_string(execTimeMin);
	}
	else {
		strMin = to_string(execTimeMin);
	}

	if (execTimeSec < 10) {
		strSec = "0" + to_string(execTimeSec);
	}
	else {
		strSec = to_string(execTimeSec);
	}

	// 출력
	string playTimeStr = strMin + ":" + strSec;
	string numCurrentConnectionStr = to_string(numCurrentConnection) + "     ";
	string numThreadStr = to_string(numActiveThread) + " / " + to_string(numBlockedThread);
	string totalReceivedBytesStr = to_string(totalReceivedBytes);
	string totalSentBytesStr = to_string(totalSentBytes);
	string receivedBpsStr;
	string sentBpsStr;
	if (execTimeTotalSec == 0) {
		receivedBpsStr = to_string(totalReceivedBytes / 1);
		sentBpsStr = to_string(totalSentBytes / 1);
	}
	else {
		receivedBpsStr = to_string(totalReceivedBytes / execTimeTotalSec);
		sentBpsStr = to_string(totalSentBytes / execTimeTotalSec);
	}

	PrintStr(XPOS_INFO_START + (short)strlen(CHAR_INFO_PLAYTIME), YPOS_INFO_TITLE + 2, playTimeStr.c_str(), COLOR_TEXT_WHITE);
	PrintStr(XPOS_INFO_START + (short)strlen(CHAR_INFO_USER_COUNT), YPOS_INFO_TITLE + 4, numCurrentConnectionStr.c_str(), COLOR_TEXT_WHITE);
	PrintStr(XPOS_INFO_START + (short)strlen(CHAR_INFO_THREAD_COUNT), YPOS_INFO_TITLE + 6, numThreadStr.c_str(), COLOR_TEXT_WHITE);
	PrintStr(XPOS_INFO_START + (short)strlen(CHAR_INFO_TOTAL_RECEIVED_BYTES), YPOS_INFO_TITLE + 8, totalReceivedBytesStr.c_str(), COLOR_TEXT_WHITE);
	PrintStr(XPOS_INFO_START + (short)strlen(CHAR_INFO_TOTAL_SENT_BYTES), YPOS_INFO_TITLE + 10, totalSentBytesStr.c_str(), COLOR_TEXT_WHITE);
	PrintStr(XPOS_INFO_START + (short)strlen(CHAR_INFO_RECEIVED_BPS), YPOS_INFO_TITLE + 12, receivedBpsStr.c_str(), COLOR_TEXT_WHITE);
	PrintStr(XPOS_INFO_START + (short)strlen(CHAR_INFO_SENT_BPS), YPOS_INFO_TITLE + 14, sentBpsStr.c_str(), COLOR_TEXT_WHITE);
}

void ConsoleController::SetInvisibleCursor()
{
	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(mStdHandle, &cursorInfo);
	cursorInfo.bVisible = FALSE;
	SetConsoleCursorInfo(mStdHandle, &cursorInfo);
}

void ConsoleController::SetVisibleCursor()
{
	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(mStdHandle, &cursorInfo);
	cursorInfo.bVisible = TRUE;
	SetConsoleCursorInfo(mStdHandle, &cursorInfo);
}

void ConsoleController::SetScreenSize(const unsigned int cols, const unsigned int line)
{
	assert(cols < 1000 && line < 500 && "SetScreenSize() - too big size.");

	string str = "mode con:cols=" + to_string(cols) + " lines=" + to_string(line);
	system(str.c_str());
}

void ConsoleController::ClearScreen()
{
	system("cls");
}

int ConsoleController::PrintStr(const short x, const short y, const char* str, const WORD color)
{
	const WORD oldColor = getCurrentColor();

	SetCursor(x, y);
	SetColor(color);
	cout << str;
	SetColor(oldColor);

	return strlen(str);
}

void ConsoleController::SetCursor(const short x, const short y)
{
	COORD position = { x,y };
	SetConsoleCursorPosition(mStdHandle, position);
}

void ConsoleController::SetColor(const WORD color)
{
	SetConsoleTextAttribute(mStdHandle, color);
}

WORD ConsoleController::getCurrentColor()
{
	CONSOLE_SCREEN_BUFFER_INFO screenInfo;
	GetConsoleScreenBufferInfo(mStdHandle, &screenInfo);
	return screenInfo.wAttributes;
}

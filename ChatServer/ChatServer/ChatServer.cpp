#include "stdafx.h"
#include "ConsoleController.h"
#include "Logger.h"
#include "NetworkController.h"
#include "Statistics.h"

#define EXT
#include "CriticalSection.h"

#define KEY_EXIT (27)

int main()
{
	InitializeCriticalSection(&lock);
	Statistics* stats = Statistics::GetInstance();
	stats->IncreaseThreadNum();

	// log file 준비
	OpenFile("log.txt");
	
	// networking 준비
	NetworkController nc;
	nc.InitWinsock();
	nc.CreateListenSocket();
	nc.Bind();
	nc.StartListening();
	nc.InitIocpController();
	nc.WaitForAccept();

	// 통계수치 출력 준비
	ConsoleController cc;
	cc.Init();
	cc.PrintInitialState();
	cc.StartPrintThread();

	// esc키를 받으면 서버 종료
	while (true) {
		int ch = _getch();

		if (ch == KEY_EXIT) {
			cc.Quit();
			nc.CloseServer();
			CloseFile();
			break;
		}
	}

	DeleteCriticalSection(&lock);
	stats->DecreaseThreadNum();

	cout << "END" << endl;
    return 0;
}

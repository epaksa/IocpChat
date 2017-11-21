#pragma once

// network config
constexpr unsigned int DEFAULT_PORT = 15000;
constexpr unsigned int SIZE_SOCKET_BUFFER = 1024 * 4;

// data
constexpr unsigned int PRINT_INTERVAL_MS = 500;	// 0.5초마다 print

// position in console
constexpr unsigned short DEFAULT_COLS = 80;
constexpr unsigned short DEFAULT_LINE = 25;

constexpr unsigned short XSIZE_EMPTY = 2;
constexpr unsigned short YSIZE_EMPTY = 1;

constexpr unsigned short XPOS_INFO_TITLE = XSIZE_EMPTY + 30;	// '== 정보 ==' 문자 시작위치
constexpr unsigned short YPOS_INFO_TITLE = 1;
constexpr unsigned short XPOS_INFO_START = XSIZE_EMPTY + 5;	// 왼쪽 정렬된 좌표

// constant string
constexpr char* CHAR_EMPTY = "  ";

constexpr char* CHAR_INFO_PLAYTIME = "Executtion time(mm:ss) : ";
constexpr char* CHAR_INFO_USER_COUNT = "Connection count : ";
constexpr char* CHAR_INFO_THREAD_COUNT = "Thread count(active/blocked) : ";
constexpr char* CHAR_INFO_TOTAL_RECEIVED_BYTES = "Total bytes received : ";
constexpr char* CHAR_INFO_TOTAL_SENT_BYTES = "Total bytes sent : ";
constexpr char* CHAR_INFO_RECEIVED_BPS = "Received bytes per second : ";
constexpr char* CHAR_INFO_SENT_BPS = "Sent bytes per second : ";
constexpr char* CHAR_INFO_FILE = "File where client's message is written : ";

// text color
constexpr unsigned short COLOR_TEXT_BLACK = 0x00;
constexpr unsigned short COLOR_TEXT_DARK_BLUE = 0x01;
constexpr unsigned short COLOR_TEXT_DARK_GREEN = 0x02;
constexpr unsigned short COLOR_TEXT_DARK_MINT = 0x03;
constexpr unsigned short COLOR_TEXT_DARK_RED = 0x04;
constexpr unsigned short COLOR_TEXT_DARK_PINK = 0x05;
constexpr unsigned short COLOR_TEXT_DARK_YELLOW = 0x06;
constexpr unsigned short COLOR_TEXT_DARK_WHITE = 0x07;
constexpr unsigned short COLOR_TEXT_GREY = 0x08;
constexpr unsigned short COLOR_TEXT_BLUE = 0x09;
constexpr unsigned short COLOR_TEXT_GREEN = 0x0a;
constexpr unsigned short COLOR_TEXT_MINT = 0x0b;
constexpr unsigned short COLOR_TEXT_RED = 0x0c;
constexpr unsigned short COLOR_TEXT_PINK = 0x0d;
constexpr unsigned short COLOR_TEXT_YELLOW = 0x0e;
constexpr unsigned short COLOR_TEXT_WHITE = 0x0f;

// background color
constexpr unsigned short COLOR_BACKGROUND_BLACK = 0x00;
constexpr unsigned short COLOR_BACKGROUND_DARK_BLUE = 0x10;
constexpr unsigned short COLOR_BACKGROUND_DARK_GREEN = 0x20;
constexpr unsigned short COLOR_BACKGROUND_DARK_MINT = 0x30;
constexpr unsigned short COLOR_BACKGROUND_DARK_RED = 0x40;
constexpr unsigned short COLOR_BACKGROUND_DARK_PINK = 0x50;
constexpr unsigned short COLOR_BACKGROUND_DARK_YELLOW = 0x60;
constexpr unsigned short COLOR_BACKGROUND_DARK_WHITE = 0x70;
constexpr unsigned short COLOR_BACKGROUND_GREY = 0x80;
constexpr unsigned short COLOR_BACKGROUND_BLUE = 0x90;
constexpr unsigned short COLOR_BACKGROUND_GREEN = 0xa0;
constexpr unsigned short COLOR_BACKGROUND_MINT = 0xb0;
constexpr unsigned short COLOR_BACKGROUND_RED = 0xc0;
constexpr unsigned short COLOR_BACKGROUND_PINK = 0xd0;
constexpr unsigned short COLOR_BACKGROUND_YELLOW = 0xe0;
constexpr unsigned short COLOR_BACKGROUND_WHITE = 0xf0;

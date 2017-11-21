#pragma once

// process config
constexpr int NUM_THREAD = 8;
constexpr int MAX_USER = 10000;
constexpr int MESSAGE_COUNT = 10;

// network config
constexpr char* IP_SERVER = "127.0.0.1";
constexpr u_short PORT_SERVER = 15000;
constexpr unsigned int SIZE_SOCKET_BUFFER = 1024 * 4;

// data
constexpr unsigned int XFIELD_SIZE = 50;
constexpr unsigned int YFIELD_SIZE = 50;
constexpr unsigned int FIELD_SIZE = XFIELD_SIZE * YFIELD_SIZE;

constexpr int GOAL = -2;
constexpr unsigned int XPOS_GOAL = 25;
constexpr unsigned int YPOS_GOAL = 24;

#pragma once

enum eDirection {
	Up,
	Down,
	Left,
	Right
};

enum eField {
	Empty = 0,
	Wall = -1,
	Goal = -2
};

enum eGameState {
	Playing,
	Finished
};

enum eLogLevel {
	Info,
	Error,
	None
};

#pragma once

#include <string>

typedef uint64_t PlayerID;

class Player {
public:
	std::string name;
	PlayerID steamId;
	bool ready;
};

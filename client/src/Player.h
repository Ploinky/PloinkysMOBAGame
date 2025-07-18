#pragma once

#include <string>
#include <steam/steam_api.h>


class Player {
public:
	std::string name;
	CSteamID steamId;
	bool ready;
};

#pragma once

#include <string>
#include "steam/steam_api.h"

struct Server_t {
	std::string name;
	servernetadr_t addr;
};

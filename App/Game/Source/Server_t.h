#pragma once

#include <string>
#include "steam/steam_api.h"

namespace PMG {
	struct Server_t {
		std::string name;
		servernetadr_t addr;
	};
}
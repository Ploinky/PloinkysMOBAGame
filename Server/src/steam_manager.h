#pragma once
#include <stddef.h>
#include "steam/steam_gameserver.h"

namespace PMG {
	class SteamManager {
	public:
		bool Initialize();
		void Shutdown();
		void RunCallbacks();
	};
}
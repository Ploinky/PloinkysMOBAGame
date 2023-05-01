#include "steam_manager.h"
#include "steam/steam_api.h"

namespace PMG {
	bool SteamManager::Initialize() {
		return SteamAPI_Init();
	};
}
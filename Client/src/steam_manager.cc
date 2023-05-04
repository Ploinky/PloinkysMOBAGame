#include "steam_manager.h"
#include "steam/steam_api.h"

namespace PMG {
	bool SteamManager::Initialize() {
		return SteamAPI_Init();
	};

	void SteamManager::RunCallbacks() {
		SteamAPI_RunCallbacks();
	}

	std::string SteamManager::GetLaunchParameter(std::string param) {
		char* str = new char[4096];
		// return std::string(SteamApps()->GetLaunchQueryParam(param.c_str()));
		SteamApps()->GetLaunchCommandLine(str, 4096);
		return std::string(str);
	}
}
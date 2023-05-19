#include "steam_manager.h"
#include "steam/steam_api.h"

namespace PMG {
	bool SteamManager::Initialize() {
		if (!SteamAPI_Init()) {
			return false;
		}

		SteamNetworkingUtils()->InitRelayNetworkAccess();
		SteamNetworkingSockets()->InitAuthentication();

		return true;
	};

	void SteamManager::RunCallbacks() {
		SteamAPI_RunCallbacks();
	}

	std::string SteamManager::GetLaunchParameter(std::string param) {
		return std::string(SteamApps()->GetLaunchQueryParam(param.c_str()));
	}
}
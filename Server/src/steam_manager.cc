#include "steam_manager.h"
#include <iostream>
#include "logger.h"
#include "steam/isteamnetworkingsockets.h" 
#include "steam/steamclientpublic.h"

namespace PMG {
	bool SteamManager::Initialize() {
		if (!SteamGameServer_Init(0, 23119, 27016, EServerMode::eServerModeAuthentication, "1.0.0.0")) {
			return false;
		}

		SteamGameServer()->SetProduct("Ploinky's MOBA Game");
		SteamGameServer()->SetGameDescription("Ploinky's MOBA Game");
		SteamGameServer()->SetServerName("Ploinky's Server");
		SteamGameServer()->SetDedicatedServer(true);
		SteamGameServer()->LogOnAnonymous();
		Logger::Msg(std::to_string(SteamGameServer()->GetSteamID().ConvertToUint64()));
		SteamGameServer()->SetAdvertiseServerActive(true);

		return true;
	}

	void SteamManager::Shutdown() {
		SteamGameServer_Shutdown();
	}

	void SteamManager::RunCallbacks() {
		SteamGameServer_RunCallbacks();
	}
}
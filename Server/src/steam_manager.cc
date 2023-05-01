#include "steam_manager.h"
#include "steam/steam_gameserver.h"
#include <iostream>
namespace PMG {
	bool SteamManager::Initialize() {
		if (!SteamGameServer_Init(0x7f000001, 23119, 27016, EServerMode::eServerModeAuthentication, "0.0.0.0")) {
			return false;
		}

		SteamGameServer()->SetProduct("Ploinky's MOBA Game");
		SteamGameServer()->SetGameDescription("Server?!");
		SteamGameServer()->SetServerName("agkljrgl");
		SteamGameServer()->SetDedicatedServer(true);
		SteamGameServer()->SetAdvertiseServerActive(true);
		SteamGameServer()->LogOnAnonymous();
		std::cout << SteamGameServer_GetSteamID() << std::endl;

		return true;
	}

	void SteamManager::Shutdown() {
		SteamGameServer_Shutdown();
	}

	void SteamManager::RunCallbacks() {
		SteamAPI_RunCallbacks();
	}
}
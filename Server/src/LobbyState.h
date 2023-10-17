#pragma once

#include "IServerState.h"
#include "NetworkManager.h"
#include "steam/steam_api.h"
#include "LobbyPlayer.h"
#include "pmg_networking.h"
#include <functional>

namespace PMG {
	class LobbyState : public IServerState {
	public:
		LobbyState();
		virtual void Update(float dt) override;

		int GetPlayerCount();

	private:
		ServerNetworkManager networkManager_;
		Networking::NetworkHandlerManager<Networking::PacketType, std::function<void(std::vector<uint8_t>, HSteamNetConnection)>> handler_;

		LobbyPlayer* lobbySlots_[10]{ 0 };

		bool CheckAllReady();
	};
}
#pragma once

#include "IServerState.h"
#include "NetworkManager.h"
#include "steam/steam_api.h"
#include "LobbyPlayer.h"

namespace PMG {
	class LobbyState : public IServerState {
	public:
		LobbyState();
		virtual void Update(float dt) override;

		int GetPlayerCount();

	private:
		ServerNetworkManager networkManager_;

		LobbyPlayer* lobbySlots_[10]{ 0 };
	};
}
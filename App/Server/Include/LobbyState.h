#pragma once

#include "IServerStateHandler.h"
#include "IServerState.h"
#include "NetworkManager.h"
#include "steam/steam_api.h"
#include "LobbyPlayer.h"
#include "Common/pmg_networking.h"
#include <functional>

namespace PMG {
	class LobbyState : public IServerState {
	public:
		LobbyState(IServerStateHandler* handler);
		LobbyState(IServerStateHandler* handler, ServerNetworkManager* networkManager);
		virtual void Update(float dt) override;

		int GetPlayerCount();

	private:
		ServerNetworkManager* networkManager_;
		Networking::NetworkHandlerManager<Networking::PacketType, std::function<void(std::vector<uint8_t>, PlayerID)>> packetHandler_;

		void Initialize();

		LobbyPlayer* lobbySlots_[10]{ 0 };

		bool CheckAllReady();
	};
}
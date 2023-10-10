#pragma once

#include "IServerState.h"
#include "NetworkManager.h"

namespace PMG {
	class LobbyPlayer {
	public:
		unsigned long socket;
		std::string name;
	};

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
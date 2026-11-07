#pragma once

#include "IServerStateHandler.h"
#include "IServerState.h"
#include "NetworkManager.h"
#include "LobbyPlayer.h"
#include "common/pmg_networking.h"
#include <functional>

class LobbyState : public IServerState {
public:
	LobbyState(IServerStateHandler* handler, int nPort, char* pszName);
	LobbyState(IServerStateHandler* handler, int nPort, char* pszName, ServerNetworkManager* networkManager);
	LobbyState(IServerStateHandler* handler, ServerNetworkManager* networkManager, LobbyPlayer* players[10]);
	virtual void Update(float dt) override;

	int GetPlayerCount();

private:
	ServerNetworkManager* networkManager_;
	NetworkHandlerManager<PacketType, std::function<void(std::vector<uint8_t>, PlayerID)>> packetHandler_;

	void Initialize(int nPort, char* pszName);
	void Initialize(LobbyPlayer* players[10]);

	LobbyPlayer* lobbySlots_[10]{ 0 };

	bool CheckAllReady();
};
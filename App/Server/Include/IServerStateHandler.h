#pragma once

namespace PMG {
	class ServerNetworkManager;
	class LobbyPlayer;

	class IServerStateHandler {
	public:
		virtual void StartGame(ServerNetworkManager* networkManager, LobbyPlayer* players[10]) = 0;
		virtual void StartLobby(ServerNetworkManager* networkManager) = 0;
	};
}
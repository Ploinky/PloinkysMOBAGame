#pragma once

class ServerNetworkManager;
class LobbyPlayer;
class CGameData;

class IServerStateHandler {
public:
	virtual void StartLobby(ServerNetworkManager* networkManager) = 0;
	virtual void StartGame(ServerNetworkManager* networkManager, LobbyPlayer* players[10]) = 0;
    virtual void ResumeLobby(ServerNetworkManager* manager, LobbyPlayer* players[10]) = 0;


	virtual const CGameData& GetGameData() = 0;
};
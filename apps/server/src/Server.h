#pragma once

#include <string>
#include <list>
#include <common/PMG_Common.h>
#include <common/pmg_networking.h>
#include "IServerState.h"
#include "IServerStateHandler.h"
#include <data/server-game-data.h>
#include <data/server-data-loader.h>

class ServerNetworkManager;
class Client;

class Server : public IServerStateHandler{
public:
    void Start();

    virtual void StartGame(ServerNetworkManager* manager, LobbyPlayer* players[10]) override;
    virtual void StartLobby(ServerNetworkManager* manager) override;

	virtual CServerGameData* GetGameData() override;
private:
    IServerState* currentState_;
    CServerDataLoader assetManager_;
    CServerGameData m_gameData;

    long long GetSystemTime();
};
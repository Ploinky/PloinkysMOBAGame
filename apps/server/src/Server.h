#pragma once

#include <string>
#include <list>
#include <common/PMG_Common.h>
#include <common/pmg_networking.h>
#include "IServerState.h"
#include "IServerStateHandler.h"
#include <common/data/game-data.h>
#include <data/server-data-loader.h>

class ServerNetworkManager;
class Client;

class Server : public IServerStateHandler{
public:
    void Start();

    virtual void StartGame(ServerNetworkManager* manager, LobbyPlayer* players[10]) override;
    virtual void StartLobby(ServerNetworkManager* manager) override;

	virtual CGameData* GetGameData() override;
private:
    IServerState* currentState_;
    CServerDataLoader assetManager_;
    CGameData m_gameData;

    long long GetSystemTime();
};
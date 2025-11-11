#pragma once

#include <string>
#include <list>
#include <common/PMG_Common.h>
#include <common/pmg_networking.h>
#include "IServerState.h"
#include "IServerStateHandler.h"
#include <data/server-game-data.h>

class ServerNetworkManager;
class Client;

class Server : public IServerStateHandler{
public:
    void Start();

    virtual void StartGame(ServerNetworkManager* manager, LobbyPlayer* players[10]) override;
    virtual void StartLobby(ServerNetworkManager* manager) override;

private:
    IServerState* currentState_;
    AssetManager assetManager_;
    CServerGameData m_gameData;

    long long GetSystemTime();
};
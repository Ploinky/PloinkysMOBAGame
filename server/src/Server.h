#pragma once 

#include <string>
#include <list>
#include <Common/PMG_Common.h>
#include <Common/pmg_networking.h>
#include "steam/steam_gameserver.h"
#include "IServerState.h"
#include "IServerStateHandler.h"

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

    long long GetSystemTime();
};
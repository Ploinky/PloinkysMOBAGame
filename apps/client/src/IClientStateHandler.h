#pragma once

#include "Renderer.h"
#include <common/PMG_Common.h>
#include "Player.h"
#include "core/platform/cursor-id.h"
#include "core/network/network-engine.h"
#include "core/network/gamer-service.h"

class IClientState;
class ClientNetworkManager;

enum class ENextState {
    NONE,
    GAME,
    LOBBY,
    SETTINGS,
    MAIN_MENU,
    SERVER_BROWSER
};

typedef struct NextState_s {
    ENextState eNextState = ENextState::NONE;
    ClientNetworkManager* networkManager;
    Player** ppPlayers;
    std::string addr;
    int port;
} NextState_t;

class IClientStateHandler {
public:
    virtual void NewState(IClientState* newState) = 0;

    virtual void JoinGame(ClientNetworkManager* networkManager) = 0;
    virtual void JoinLobby(std::string addr, int port) = 0;

    virtual void StartCharacterSelect(ClientNetworkManager* networkManager, Player** ppPlayers) = 0;

    virtual void OpenSettingsMenu() = 0;
    virtual void OpenMainMenu() = 0;
    virtual void OpenServerBrowser() = 0;

    virtual void RequestCursor(CursorId newId) = 0;
		
	virtual CRenderer* GetRenderer() = 0;
	virtual CClientAssetManager* GetAssetManager() = 0;

    virtual IAudioEngine* GetAudioEngine() = 0;
    virtual INetworkEngine* GetNetworkEngine() = 0;
    virtual IGamerService* GetGamerService() = 0;
};

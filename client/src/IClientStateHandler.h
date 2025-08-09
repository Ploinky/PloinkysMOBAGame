#pragma once

#include "steam/steam_api.h"
#include <Renderer.h>
#include <Common/PMG_Common.h>
#include <Player.h>

enum class CursorId {
    DEFAULT,
    BUTTON_HOVER,

    // INGAME
    ATTACK_MOVE
};
class IClientState;
class CAudioEngine;
class ClientNetworkManager;

class IClientStateHandler {
public:
    virtual void NewState(IClientState* newState) = 0;

    virtual void JoinGame(ClientNetworkManager* networkManager) = 0;
    virtual void JoinLobby(servernetadr_t addr) = 0;

    virtual void StartCharacterSelect(ClientNetworkManager* networkManager, Player** ppPlayers) = 0;

    virtual void OpenSettingsMenu() = 0;
    virtual void OpenMainMenu() = 0;
    virtual void OpenServerBrowser() = 0;

    virtual void RequestCursor(CursorId newId) = 0;
		
	virtual CRenderer* GetRenderer() = 0;
	virtual CClientAssetManager* GetAssetManager() = 0;

    virtual CAudioEngine* GetAudioEngine() = 0;
};

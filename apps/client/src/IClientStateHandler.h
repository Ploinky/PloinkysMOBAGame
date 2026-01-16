#pragma once

#include <Renderer.h>
#include <common/PMG_Common.h>
#include <Player.h>
#include <core/platform/cursor-id.h>
#include "core/network/network-engine.h"

class IClientState;
class ClientNetworkManager;

class IClientStateHandler {
public:
    virtual void NewState(IClientState* newState) = 0;

    virtual void JoinGame(ClientNetworkManager* networkManager) = 0;
    virtual void JoinLobby(const char* addr, int port) = 0;

    virtual void StartCharacterSelect(ClientNetworkManager* networkManager, Player** ppPlayers) = 0;

    virtual void OpenSettingsMenu() = 0;
    virtual void OpenMainMenu() = 0;
    virtual void OpenServerBrowser() = 0;

    virtual void RequestCursor(CursorId newId) = 0;
		
	virtual CRenderer* GetRenderer() = 0;
	virtual CClientAssetManager* GetAssetManager() = 0;

    virtual IAudioEngine* GetAudioEngine() = 0;
    virtual INetworkEngine* GetNetworkEngine() = 0;
};

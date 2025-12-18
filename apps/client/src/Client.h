#pragma once

#include <vector>
#include <list>
#include <string>
#include <map>
#include "AudioSystem.h"
#include "Settings.h"
#include "common/pmg_physics.h"
#include <common/PMG_Common.h>
#include "IClientStateHandler.h"
#include <LoadingState.h>
#include <CharacterSelectState.h>
#include <core/input/input-manager.h>

// Main game application
class Mesh;
class CD3D11GraphicsEngine;
class CRenderer;
class Window;
class KeyboardInput;
class MouseInput;
class NetworkConnection;
class Map;
class CRenderer;
class Window;
class Scene;
class ClientStateHandler;
class GuiElement;
class GuiTextfield;
class IClientState;
class ClientNetworkManager;
class INetworkEngine;

class Client : public IClientStateHandler {

public:
    Client();
    ~Client();
    void Run(std::string connectString_);

    void NewState(IClientState* newState);
    virtual void JoinGame(ClientNetworkManager* networkManager) override;
    virtual void JoinLobby(const char* addr) override;
    virtual void OpenSettingsMenu() override;
    virtual void OpenMainMenu() override;
    virtual void OpenServerBrowser() override;
    virtual void StartCharacterSelect(ClientNetworkManager* networkManager, Player** ppPlayers) override;

    virtual void RequestCursor(CursorId newId) override;

	virtual CRenderer* GetRenderer() override;
	virtual CClientAssetManager* GetAssetManager() override;

    virtual IAudioEngine* GetAudioEngine() override;
    virtual INetworkEngine* GetNetworkEngine() override;

private:
    IClientState* currentState_;

    // Indicates whether the Client is running and should continue running
    bool isRunning;

    // Access to native window 
    Window* window = nullptr;
    // Access to DirectX 11 rendering pipeline
    CRenderer* renderer = nullptr;

    long long lastFrame;

    // Clears the screen and prepares rendering
    void BeginRender();
    // Final rendering operations and presents the rendered image to the screen
    void FinishRender();

    void HandleSettingChanged(std::string setting);

    Settings settings_;
    IAudioEngine* m_pAudioEngine;
    IGraphicsEngine* m_pGraphicsEngine;
    INetworkEngine* m_pNetworkEngine;
    CClientAssetManager* m_pAssetManager;
    CInputManager* m_pInputManager;

    // Mouse input

    int m_sceneWidth = 0;
    int m_sceneHeight = 0;

    int fps;
};

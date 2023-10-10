#pragma once

#include <vector>
#include <list>
#include <string>
#include <map>
#include "AudioSystem.h"
#include "Settings.h"
#include "pmg_physics.h"
#include "AssetManager.h"
#include "IClientStateHandler.h"
#include "steam/steam_api.h"

// Main game application
namespace PMG {
    class Mesh;
    class Direct3D;
    class Renderer;
    class Window;
    class KeyboardInput;
    class MouseInput;
    class NetworkConnection;
    class Map;
    class Renderer;
    class Window;
    class Scene;
    class ClientStateHandler;
    class GuiElement;
    class GuiTextfield;
    class IClientState;

    class Client : public IClientStateHandler {

    public:
        Client();
        ~Client();
        void Run();

        void NewState(IClientState* newState);
        virtual void JoinGame(servernetadr_t addr) override;
        virtual void JoinLobby(servernetadr_t addr) override;
        virtual void OpenSettingsMenu() override;
        virtual void OpenMainMenu() override;
        virtual void OpenServerBrowser() override;

        STEAM_CALLBACK(Client, GameServerChangeRequested, GameServerChangeRequested_t);

    private:
        IClientState* currentState_;

        // Indicates whether the Client is running and should continue running
        bool isRunning;

        // Access to DirectX 11 resources
        Direct3D* direct3D = nullptr;
        // Access to native window 
        Window* window = nullptr;
        // Access to DirectX 11 rendering pipeline
        Renderer* renderer = nullptr;

        long long lastFrame;

        // Clears the screen and prepares rendering
        void BeginRender();
        // Final rendering operations and presents the rendered image to the screen
        void FinishRender();

        void HandleSettingChanged(std::string setting);

        Settings settings_;
        AudioSystem audio_system_;
        AssetManager assetManager_;

        // Mouse input

        int m_sceneWidth = 0;
        int m_sceneHeight = 0;

        int fps;
    };
}
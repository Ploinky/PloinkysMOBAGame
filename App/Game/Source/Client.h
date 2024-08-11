#pragma once

#include <vector>
#include <list>
#include <string>
#include <map>
#include "AudioSystem.h"
#include "Settings.h"
#include "Common/pmg_physics.h"
#include <Common/PMG_Common.h>
#include "IClientStateHandler.h"
#include "steam/steam_api.h"
#include <LoadingState.h>

// Main game application
namespace PMG {
    class Mesh;
    class Direct3D;
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
    class ServerNetworkManager;

    class Client : public IClientStateHandler {

    public:
        Client();
        ~Client();
        void Run(std::string connectString_);

        void NewState(IClientState* newState);
        virtual void JoinGame(ServerNetworkManager* networkManager) override;
        virtual void JoinLobby(servernetadr_t addr) override;
        virtual void OpenSettingsMenu() override;
        virtual void OpenMainMenu() override;
        virtual void OpenServerBrowser() override;

        virtual void RequestCursor(CursorId newId) override;

		virtual CRenderer* GetRenderer() override;
		virtual AssetManager* GetAssetManager() override;


        STEAM_CALLBACK(Client, GameServerChangeRequested, GameServerChangeRequested_t);
        STEAM_CALLBACK(Client, WebApiTicketReceived, GetTicketForWebApiResponse_t);

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
        AudioSystem audio_system_;
        AssetManager assetManager_;

        // User Authentication
        HAuthTicket authTicket_;

        // Mouse input

        int m_sceneWidth = 0;
        int m_sceneHeight = 0;

        int fps;
    };
}
#include "Client.h"
#include <iostream>
#include "Window.h"
#include "Vertex.h"
#include "logger.h"
#include "Direct3D.h"
#include "Renderer.h"
#include <directxmath.h>
#include "Camera.h"
#include "Map.h"
#include <sstream>
#include "util.h"
#include "navigation.h"
#include <locale>
#include "Settings.h"
#include "AudioSystem.h"
#include "Mesh.h"
#include "pmg_networking.h"
#include "ParticleSystem.h"
#include "Gui.h"
#include "MainMenu.h"
#include "steam/steam_api.h"
#include "Game.h"
#include "SettingsMenu.h"
#include "ServerBrowser.h"
#include "Lobby.h"

namespace PMG {
    enum class GameState {
        MAIN_MENU,
        GAMEPLAY,
    };

    Client::Client() {
        isRunning = false;
        lastFrame = 0;

        fps = 0;
    }

    Client::~Client() {
        if (renderer != nullptr) {
            delete renderer;
            renderer = 0;
        }

        if (direct3D != nullptr) {
            delete direct3D;
            direct3D = 0;
        }

        if (window != nullptr) {
            delete window;
            window = 0;
        }
    }

    void Client::Run() {
        Logger::Msg("Starting Ploinky's MOBA Game client...");
        Logger::Msg("Loading settings...");
        settings_.LoadDefaults();
        settings_.LoadFromFile("./settings.cfg");
        settings_.OnSettingChanged = [this](std::string setting) {
            HandleSettingChanged(setting);
        };

        auto devModeIt = settings_.GetAllVideoModesAndValues()->find(settings_.GetString(PMGSettings::VIDEO_MODE));
        
        int width = 1024;
        int height = 768;
        
        if (devModeIt != settings_.GetAllVideoModesAndValues()->end()) {

            width = devModeIt->second.dmPelsWidth;
            height = devModeIt->second.dmPelsHeight;
        }
        // Create and show window
        window = new Window(
            width,
            height,
            (WindowMode)settings_.GetInt(PMGSettings::WINDOW_MODE)
        );
        window->Show();

        // ----- Initialize Direct3D -----
        // Must be done after window is showing, otherwise swap chain creation fails
        direct3D = new Direct3D();

        // Immediately close window if initialization of Direct3D fails
        if (!direct3D->Initialize(window->GetWindowHandle(), settings_.GetInt(PMGSettings::WINDOW_MODE) == (int)WindowMode::FULLSCREEN)) {
            Logger::Err("Direct3D initialization failed, PMG will quit.");
            window->SetShouldClose();
        }

        window->windowResizedHandler = [this]() {
            if (window->ShouldClose()) {
                return;
            }

            direct3D->SetWindowDimensions(window->width_, window->height_);
            renderer->SetDimensions(window->width_, window->height_);
            if (currentState_) currentState_->WindowResized(window->width_, window->height_);
        };

        window->e_charTyped = [this](WORD ch) { if(currentState_) currentState_->CharTyped(ch); };
        window->e_keyPressed = [this](WORD key) { if (currentState_) currentState_->KeyPressed(key); };
        window->e_keyReleased = [this](WORD key) { if (currentState_) currentState_->KeyReleased(key); };
        window->e_mouseButtonPressed = [this](int button) { if (currentState_) currentState_->MouseButtonPressed(button); };
        window->e_mouseButtonReleased = [this](int button) { if (currentState_) currentState_->MouseButtonReleased(button); };
        window->e_mouseMoved = [this](int x, int y) { if (currentState_) currentState_->MouseMoved(x, y); };

        renderer = new Renderer();
        renderer->Initialize(direct3D, window->width_, window->height_);

        currentState_ = new MainMenu(this, window->width_, window->height_);

        std::vector<PMGSystem*> systems;

        std::wstring strFileName = TEXT("audio.wav");
        AudioComponent comp;
        comp.fileName = strFileName;
        comp.isPlaying = false;

        systems.push_back(&audio_system_);

        // Main game loop
        // Keep running while both the client wants to keep runnning and the window has not been closed
        isRunning = true;
        lastFrame = Util::GetSystemTime();

        Logger::Msg("Starting main game loop");
        while (isRunning && currentState_ && !window->ShouldClose()) {
            auto thisFrame = Util::GetSystemTime();
            float dt = static_cast<float>(thisFrame - lastFrame);
            lastFrame = thisFrame;

            m_sceneWidth = window->width_;
            m_sceneHeight = window->height_;

            // Event handling
            window->HandleEvents();

            if (!currentState_) {
                continue;
            }

            currentState_->Update(dt);

            BeginRender();
            currentState_->Render(renderer);
            FinishRender();

            for (PMGSystem* system : systems) {
                system->Update();
            }

            SteamAPI_RunCallbacks();

            fps = (int)(1000.0f / dt);
        }

        // Save settings to settings file
        settings_.SaveToFile("./settings.cfg");

        // Game has endeded, close window if it isn't already closing
        if (!window->ShouldClose()) {
            window->SetShouldClose();
        }

        Logger::Msg("Game loop has been stopped.");
    }

    void Client::BeginRender() {  
        direct3D->ClearScreen();
        renderer->UpdateCameraMatrix();
    }

    void Client::FinishRender() {
        direct3D->Present();
    }

    void Client::HandleSettingChanged(std::string setting) {
        if (setting == PMGSettings::MASTER_VOLUME) {
            audio_system_.SetMasterVolume(settings_.GetDouble(PMGSettings::MASTER_VOLUME));
        }
        else if (setting == PMGSettings::WINDOW_MODE) {
            DEVMODEA mode = settings_.GetAllVideoModesAndValues()->find(settings_.GetString(PMGSettings::VIDEO_MODE))->second;
            window->SetWindowMode(static_cast<WindowMode>(settings_.GetInt(PMGSettings::WINDOW_MODE)), mode.dmPelsWidth, mode.dmPelsHeight);
            direct3D->SetFullScreen(settings_.GetInt(PMGSettings::WINDOW_MODE) == static_cast<int>(WindowMode::FULLSCREEN));
            direct3D->SetWindowDimensions(window->width_, window->height_);
        }
        else if (setting == PMGSettings::VIDEO_MODE) {
            DEVMODEA mode = settings_.GetAllVideoModesAndValues()->find(settings_.GetString(PMGSettings::VIDEO_MODE))->second;
            window->SetWindowMode(static_cast<WindowMode>(settings_.GetInt(PMGSettings::WINDOW_MODE)), mode.dmPelsWidth, mode.dmPelsHeight);
            direct3D->SetFullScreen(settings_.GetInt(PMGSettings::WINDOW_MODE) == static_cast<int>(WindowMode::FULLSCREEN));
            direct3D->SetWindowDimensions(window->width_, window->height_);
        }
    }

    void Client::NewState(IClientState* clientState) {
        delete currentState_;
        currentState_ = clientState;
    }

    void Client::GameServerChangeRequested(GameServerChangeRequested_t* callback) {
        printf("requested");

        NewState(new Lobby(callback->m_rgchServer, this, window->width_, window->height_));
    }

    void Client::JoinGame(servernetadr_t addr) {
        Game* game = new Game(addr.GetConnectionAddressString(), this, window->width_, window->height_, renderer, &assetManager_);
        NewState(game);
    };

    void Client::JoinLobby(servernetadr_t addr) {
        Lobby* lobby = new Lobby(addr.GetConnectionAddressString(), this, window->width_, window->height_);
        NewState(lobby);
    }

    void Client::OpenSettingsMenu() {
        SettingsMenu* menu = new SettingsMenu(this, window->width_, window->height_, &settings_);
        NewState(menu);
    }

    void Client::OpenMainMenu() {
        MainMenu* mainMenu = new MainMenu(this, window->width_, window->height_);
        NewState(mainMenu);
    }

    void Client::OpenServerBrowser() {
        ServerBrowser* serverBrowser = new ServerBrowser(this, window->width_, window->height_);
        NewState(serverBrowser);
    }
}

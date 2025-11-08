#include "Client.h"
#include <iostream>
#include <core/window/Window.h>
#include <common/PMG_Common.h>
#include "Renderer.h"
#include "Camera.h"
#include <sstream>
#include <common/PMG_Common.h>
#include <locale>
#include "Settings.h"
#include "AudioSystem.h"
#include "common/pmg_networking.h"
#include "ParticleEffect.h"
#include "Gui.h"
#include "MainMenu.h"
#include <Game.h>
#include "SettingsMenu.h"
#include "ServerBrowser.h"
#include "Lobby.h"
#include <string>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <limits>
#include "core/audio/audio-engine.h"
#include <core/platform/platform.h>

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
    m_pAssetManager->Cleanup();

    if (renderer != nullptr) {
        delete renderer;
        renderer = 0;
    }

    if (window != nullptr) {
        delete window;
        window = 0;
    }
}

void Client::Run(std::string connectString_) {
    Logger::Msg("Starting Ploinky's MOBA Game client...");

    Logger::Msg("Authenticating user session...");

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

        width = devModeIt->second.uWidth;
        height = devModeIt->second.uHeight;
    }
    // Create and show window
    window = Window::Create(width, height, (WindowMode)settings_.GetInt(PMGSettings::WINDOW_MODE));
    window->Show();

    window->windowResizedHandler = [this]() {
        if (window->ShouldClose()) {
            return;
        }

        renderer->SetDimensions(window->width_, window->height_);
        if (currentState_) currentState_->WindowResized(window->width_, window->height_);
    };

    m_pGraphicsEngine = IGraphicsEngine::Create(window->GetWindowHandle(), window->width_, window->height_);
    
    m_pAudioEngine = IAudioEngine::Create();
    m_pAudioEngine->Initialize();

    m_pAssetManager = new CClientAssetManager(m_pGraphicsEngine, m_pAudioEngine);

    renderer = new CRenderer(m_pGraphicsEngine);
	renderer->Initialize(settings_.GetInt(PMGSettings::WINDOW_MODE) == (int)WindowMode::FULLSCREEN, m_pAssetManager, window->width_, window->height_);

    m_pInputManager = new CInputManager(window);
    m_pInputManager->on_action = [this](EInputAction eAction) { if(currentState_) currentState_->Action(eAction); };
    m_pInputManager->on_action_released = [this](EInputAction eAction) { if(currentState_) currentState_->ActionReleased(eAction); };
    window->e_charTyped = [this](uint32_t ch) { if(currentState_) currentState_->CharTyped(ch); };
    // window->e_keyPressed = [this](EKeyCode eKey) { if (currentState_) currentState_->KeyPressed(eKey); };
    // window->e_keyReleased = [this](uint32_t key) { if (currentState_) currentState_->KeyReleased(key); };
    // window->e_mouseButtonPressed = [this](EMouseButton eBtn) { if (currentState_) currentState_->MouseButtonPressed(eBtn); };
    // window->e_mouseButtonReleased = [this](int button) { if (currentState_) currentState_->MouseButtonReleased(button); };
    window->e_mouseMoved = [this](int x, int y) { if (currentState_) currentState_->MouseMoved(x, y); };

	currentState_ = new CLoadingState(this, window->width_, window->height_, connectString_);
        

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

        m_pAudioEngine->Update();

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
    renderer->ClearScreen();
    renderer->UpdateCameraMatrix();
}

void Client::FinishRender() {
    renderer->Present();
}

void Client::HandleSettingChanged(std::string setting) {
    if (setting == PMGSettings::MASTER_VOLUME) {
        m_pAudioEngine->SetMasterVolume(settings_.GetDouble(PMGSettings::MASTER_VOLUME));
    }
    else if (setting == PMGSettings::WINDOW_MODE) {
        auto allModes = settings_.GetAllVideoModesAndValues();
        auto selectedMode = allModes->find(settings_.GetString(PMGSettings::VIDEO_MODE));

        if (selectedMode == allModes->end()) {
            selectedMode = allModes->begin();
        }

        VideoMode_t mode = selectedMode->second;
        renderer->SetFullscreen(settings_.GetInt(PMGSettings::WINDOW_MODE) == static_cast<int>(WindowMode::FULLSCREEN));
        window->SetWindowMode(static_cast<WindowMode>(settings_.GetInt(PMGSettings::WINDOW_MODE)), mode.uWidth, mode.uHeight);
        renderer->SetDimensions(window->width_, window->height_);
    }
    else if (setting == PMGSettings::VIDEO_MODE) {
        VideoMode_t mode = settings_.GetAllVideoModesAndValues()->find(settings_.GetString(PMGSettings::VIDEO_MODE))->second;
        renderer->SetFullscreen(settings_.GetInt(PMGSettings::WINDOW_MODE) == static_cast<int>(WindowMode::FULLSCREEN));
        window->SetWindowMode(static_cast<WindowMode>(settings_.GetInt(PMGSettings::WINDOW_MODE)), mode.uWidth, mode.uHeight);
        renderer->SetDimensions(window->width_, window->height_);
    }
}

void Client::RequestCursor(CursorId newCursor) {
    CPlatform::RequestCursor(newCursor);
}

void Client::NewState(IClientState* clientState) {
    delete currentState_;
    currentState_ = clientState;
}

void Client::StartCharacterSelect(ClientNetworkManager* networkManager, Player** ppPlayers) {
    CCharacterSelectState* select = new CCharacterSelectState(networkManager, this, window->width_, window->height_, ppPlayers);
    NewState(select);
}

void Client::JoinGame(ClientNetworkManager* networkManager) {
    Game* game = new Game(networkManager, this, window->width_, window->height_);
    NewState(game);
    m_pInputManager->SetContext(EInputContext::GAME);
};

void Client::JoinLobby(servernetadr_t addr) {
    m_pInputManager->SetContext(EInputContext::MENU);
    Lobby* lobby = new Lobby(addr.GetConnectionAddressString(), this, window->width_, window->height_);
    NewState(lobby);
}

void Client::OpenSettingsMenu() {
    m_pInputManager->SetContext(EInputContext::MENU);
    SettingsMenu* menu = new SettingsMenu(this, window->width_, window->height_, &settings_);
    NewState(menu);
}

void Client::OpenMainMenu() {
    m_pInputManager->SetContext(EInputContext::MENU);
    MainMenu* mainMenu = new MainMenu(this, window->width_, window->height_);
    NewState(mainMenu);
}

void Client::OpenServerBrowser() {
    m_pInputManager->SetContext(EInputContext::MENU);
    ServerBrowser* serverBrowser = new ServerBrowser(this, window->width_, window->height_);
    NewState(serverBrowser);
}
	
CRenderer* Client::GetRenderer() {
	return renderer;
}
	
CClientAssetManager* Client::GetAssetManager() {
	return m_pAssetManager;
}

IAudioEngine* Client::GetAudioEngine() {
    return m_pAudioEngine;
}

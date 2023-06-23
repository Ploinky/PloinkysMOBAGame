#include "client.h"
#include <iostream>
#include "window.h"
#include "mesh.h"
#include "vertex.h"
#include "logger.h"
#include "direct3d.h"
#include "renderer.h"
#include <directxmath.h>
#include "camera.h"
#include "map.h"
#include <sstream>
#include "util.h"
#include "navigation.h"
#include "scene.h"
#include <locale>
#include <codecvt>
#include "settings.h"
#include "audio_system.h"
#include "settings_scene.h"

namespace PMG {
    Client::Client() {
        isRunning = false;
        lastFrame = 0;
        m_oldState = ClientState::SHUTDOWN;
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

        std::vector<PMGSystem*> systems;

        Logger::Msg("Loading settings...");

        settings_.LoadDefaults();
        settings_.LoadFromFile("./settings.cfg");
        settings_.OnSettingChanged = [this](std::string setting) {
            HandleSettingChanged(setting);
        };

        // Create and show window
        window = new Window(
            settings_.GetInt(PMGSettings::RESOLUTION_X),
            settings_.GetInt(PMGSettings::RESOLUTION_Y),
            (WindowMode) settings_.GetInt(PMGSettings::WINDOW_MODE)
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

            direct3D->SetWindowDimensions(window->width, window->height);
            renderer->SetDimensions(window->width, window->height);
        };

        window->e_charTyped = [this](WORD ch) {
            Scene* currentScene = GetScene<Scene>(m_oldState);

            if (currentScene) {
                currentScene->CharTyped(ch);
            }
        };

        window->e_keyPressed = [this](WORD key) {
            Scene* currentScene = GetScene<Scene>(m_oldState);

            if (currentScene) {
                currentScene->KeyPressed(key);
            }
        };

        window->e_keyReleased= [this](WORD key) {
            Scene* currentScene = GetScene<Scene>(m_oldState);

            if (currentScene) {
                currentScene->KeyReleased(key);
            }
        };

        window->e_mouseButtonPressed = [this](int button) {
            Scene* currentScene = GetScene<Scene>(m_oldState);

            if (currentScene) {
                currentScene->MouseButtonPressed(button);
            }
        };

        window->e_mouseButtonReleased = [this](int button) {
            Scene* currentScene = GetScene<Scene>(m_oldState);

            if (currentScene) {
                currentScene->MouseButtonReleased(button);
            }
        };

        window->e_mouseMoved = [this](int x, int y) {
            Scene* currentScene = GetScene<Scene>(m_oldState);

            if (currentScene) {
                currentScene->MouseMoved(x, y);
            }
        };

        renderer = new Renderer();
        renderer->Initialize(direct3D, window->width, window->height);

        if (!audio_system_.Initialize()) {
            Logger::Msg("Failed to initialize audio system");
            return;
        }
        systems.push_back(&audio_system_);

        Logger::Msg("Starting main game loop");

        m_oldState = ClientState::STARTUP;
        m_stateStack.push_back(ClientState::MAIN_MENU);
        
        // TODO: this does not actually work, you know?
        std::string ip = "";
        Logger::Msg("IP:");
        Logger::Msg(ip);
        Logger::Msg("-----");

        if (ip.length() > 0) {
            m_oldState = ClientState::CONNECT;
            m_stateStack.push_back(ClientState::CONNECT);
            ConnectScene* connectScene = new ConnectScene(this, ip.substr(0, ip.find(':')));
            m_scenes[ClientState::CONNECT] = connectScene;
        }

        // Main game loop
        // Keep running while both the client wants to keep runnning and the window has not been closed
        isRunning = true;
        lastFrame = Util::GetSystemTime();
        while(isRunning && !window->ShouldClose()) {
            auto thisFrame = Util::GetSystemTime();
            float dt = static_cast<float>(thisFrame - lastFrame);
            lastFrame = thisFrame;

            ClientState currentState = m_stateStack.back();
            if (m_oldState != currentState) {
                // Entering state
                switch (currentState) {
                case ClientState::STARTUP: {
                    break;
                }
                case ClientState::NETWORKED_GAME: {
                    ConnectScene* connectScene = GetScene<ConnectScene>(ClientState::CONNECT);
                    NetworkedGame* gameScene = new NetworkedGame(this, connectScene->GetConnection());
                    gameScene->m_sceneWidth = window->width;
                    gameScene->m_sceneHeight = window->height;
                    m_scenes[ClientState::NETWORKED_GAME] = gameScene;
                    break;
                }
                case ClientState::SETTINGS: {
                    SettingsScene* settingsScene = new SettingsScene(this, &settings_);
                    settingsScene->m_sceneWidth = window->width;
                    settingsScene->m_sceneHeight = window->height;
                    m_scenes[ClientState::SETTINGS] = settingsScene;
                    break;
                }
                case ClientState::CONNECT: {
                    if (m_oldState != ClientState::MAIN_MENU) {
                        DeleteScene<ConnectScene>(ClientState::CONNECT);
                        m_stateStack.pop_back();
                        break;
                    }
                    MainMenuScene* mainMenuScene = (MainMenuScene*)m_scenes.at(ClientState::MAIN_MENU);
                    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
                    std::string narrowIp = converter.to_bytes(mainMenuScene->GetIp());
                    ConnectScene* connectScene = new ConnectScene(this, narrowIp);
                    m_scenes[ClientState::CONNECT] = connectScene;
                    break;
                }
                case ClientState::MAIN_MENU: {
                    AddScene<MainMenuScene>(ClientState::MAIN_MENU);
                    break;
                }
                case ClientState::SHUTDOWN:
                default: {
                    isRunning = false;
                    break;
                }
                }

                // Leaving state
                switch (m_oldState) {
                case ClientState::STARTUP: {
                    break;
                }
                case ClientState::NETWORKED_GAME: {
                    DeleteScene<NetworkedGame>(ClientState::NETWORKED_GAME);
                    break;
                }
                case ClientState::SETTINGS: {
                    DeleteScene<SettingsScene>(ClientState::SETTINGS);
                    break;
                }
                case ClientState::CONNECT: {
                    DeleteScene<ConnectScene>(ClientState::CONNECT);
                    break;
                }
                case ClientState::MAIN_MENU: {
                    DeleteScene<MainMenuScene>(ClientState::MAIN_MENU);
                    break;
                }
                case ClientState::SHUTDOWN:
                default: {
                    break;
                }
                }


                m_oldState = currentState;
            }

            Scene* currentScene = m_scenes[m_oldState];

            if (currentScene == nullptr) {
                // No scene, does that mean we quit?
                continue;
            }

            // Event handling
            window->HandleEvents();

            currentScene->Update(dt);

            // Render scene
            BeginRender();
            currentScene->Render(renderer);
            FinishRender();

            for (PMGSystem* system : systems) {
                system->Update();
            }
        }

        // Game has endeded, close window if it isn't already closing
        if (!window->ShouldClose()) {
            window->SetShouldClose();
        }

        // Save settings to settings file
        settings_.SaveToFile("./settings.cfg");

        Logger::Msg("Game loop has been stopped.");
    }
    
    void Client::BeginRender() {
        direct3D->ClearScreen();
        renderer->UpdateCameraMatrix();
    }

    void Client::FinishRender() {
        direct3D->Present();
    }

    void Client::PushState(ClientState nextState) {
         m_stateStack.push_back(nextState);
    }

    void Client::PopState() {
        m_stateStack.pop_back();
    }

    template<typename T>
    T* Client::GetScene(ClientState state) {
        return (T*)m_scenes[state];
    }

    template<typename T>
    T* Client::AddScene(ClientState state) {
        T* scene = new T(this);
        m_scenes[state] = scene;
        return scene;
    }

    template<typename T>
    void Client::DeleteScene(ClientState state) {
        T* scene = GetScene<T>(state);
        delete scene;
        m_scenes[state] = nullptr;
    }

    void Client::HandleSettingChanged(std::string setting) {
        if (setting == PMGSettings::MASTER_VOLUME) {
            audio_system_.SetMasterVolume(settings_.GetDouble(PMGSettings::MASTER_VOLUME));
        }
        else if (setting == PMGSettings::WINDOW_MODE) {
            window->SetWindowMode(static_cast<WindowMode>(settings_.GetInt(PMGSettings::WINDOW_MODE)), settings_.GetInt(PMGSettings::RESOLUTION_X),  settings_.GetInt(PMGSettings::RESOLUTION_Y));
            direct3D->SetFullScreen(settings_.GetInt(PMGSettings::WINDOW_MODE) == static_cast<int>(WindowMode::FULLSCREEN));
        }
    }
}
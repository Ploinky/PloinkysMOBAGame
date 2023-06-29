#pragma once

#include <vector>
#include <list>
#include <string>
#include <map>
#include "client_network_manager.h"
#include "audio_system.h"
#include "settings.h"

// Main game application
namespace PMG {
    class Mesh;
    class Direct3D;
    class Renderer;
    class Window;
    class KeyboardInput;
    class MouseInput;
    class NavMesh;
    class Scene;

    enum class ClientState {
        STARTUP,
        MAIN_MENU,
        SETTINGS,
        CONNECT,
        NETWORKED_GAME,
        SHUTDOWN,
    };

    class ClientStateHandler {
    public:
        virtual void PushState(ClientState state) {};
        virtual void PopState() {};
    };
    
    class Client : public ClientStateHandler{
        public:
            Client();
            ~Client();
            void Run();

        private:
            // Indicates whether the Client is running and should continue running
            bool isRunning;
            // Access to DirectX 11 resources
            Direct3D* direct3D = 0;
            // Access to native window 
            Window* window = 0;
            // Access to DirectX 11 rendering pipeline
            Renderer* renderer = 0;

            long long lastFrame;

            // Clears the screen and prepares rendering
            void BeginRender();
            // Render the specified 3D model to the screen
            void Render(Mesh* model);
            // Final rendering operations and presents the rendered image to the screen
            void FinishRender();

            void PushState(ClientState state);
            void PopState();

            void HandleSettingChanged(std::string setting);
            
            template <typename T>
            T* GetScene(ClientState state);

            template<typename T>
            void DeleteScene(ClientState state);

            template <typename T>
            T* AddScene(ClientState state);

            std::vector<ClientState> m_stateStack;
            ClientState m_oldState;
            std::map<ClientState, Scene*> m_scenes;

            Settings settings_;
            AudioSystem audio_system_;
    };
}
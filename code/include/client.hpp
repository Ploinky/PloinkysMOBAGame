#pragma once

#include <vector>
#include <list>
#include <string>

// Main game application
namespace P3D {
    typedef struct {
        unsigned long index;
        long long received;
        float x;
        float y;
    } game_tick_t;

    class Mesh;
    class Direct3D;
    class Renderer;
    class Window;
    class KeyboardInput;
    class MouseInput;
    class NetworkConnection;
    
    class Client {
        public:
            ~Client();
            void Run();

        private:
            // Indicates whether the Client is running and should continue running
            bool isRunning;
            // Access to DirectX 11 resources
            Direct3D* direct3D;
            // Access to native window 
            Window* window;
            // Access to DirectX 11 rendering pipeline
            Renderer* renderer;
            
            NetworkConnection* network;

            long long lastFrame;

            KeyboardInput* keyboardInput;
            MouseInput* m_mouseInput;

            std::vector<Mesh*> models;

            // Clears the screen and prepares rendering
            void BeginRender();
            // Render the specified 3D model to the screen
            void Render(Mesh* model);
            // Final rendering operations and presents the rendered image to the screen
            void FinishRender();

            void HandlePlayerInput(Mesh* model, float dt);
            void HandleNetworkMessage(std::string msg);
            void HandleTicks();
            long long GetSystemTime();

            std::list<game_tick_t> ticks;
    };
}
#pragma once

#include <vector>
#include <list>
#include <string>

// Main game application
namespace PMG {
    typedef struct {
        float x;
        float y;
        unsigned long unitId;
    } unit_t;

    typedef struct {
        unsigned long index;
        long long received;
        std::list<unit_t> units;
    } game_tick_t;

    class Mesh;
    class Direct3D;
    class Renderer;
    class Window;
    class KeyboardInput;
    class MouseInput;
    class NetworkConnection;
    class NavMesh;
    
    class Client {
        public:
            Client(std::string ip);
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

            std::vector<Mesh*> models;
            NavMesh* m_navMesh;

            // Clears the screen and prepares rendering
            void BeginRender();
            // Render the specified 3D model to the screen
            void Render(Mesh* model);
            // Final rendering operations and presents the rendered image to the screen
            void FinishRender();

            void HandlePlayerInput(Mesh* model, float dt);
            void HandleNetworkMessage(std::string msg);
            void HandleTicks(long long frameTime);
            long long GetSystemTime();

            void SpawnUnit(unsigned long id);
            void DespawnUnit(unsigned long id);

            Mesh* GetModelForUnit(unsigned long untiId);

            std::list<game_tick_t> ticks;
            std::list<unit_t> units;

            std::string ip;
    };
}
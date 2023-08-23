#pragma once

#include <vector>
#include <list>
#include <string>
#include <map>
#include "client_network_manager.h"
#include "audio_system.h"
#include "settings.h"
#include "physics.h"

// Main game application
namespace PMG {
    class Mesh;
    class Direct3D;
    class Renderer;
    class Window;
    class KeyboardInput;
    class MouseInput;
    class NavMesh;
    class Mesh;
    class NetworkConnection;
    class Map;
    class Renderer;
    class Window;
    class Scene;
    class ClientStateHandler;
    class GuiElement;
    class GuiTextfield;

    typedef struct {
        vec2_t pos;
        float rot;
        unsigned long unitId;
    } unit_t;

    typedef struct {
        unsigned long index;
        long long received;
        std::list<unit_t> units;
    } game_tick_t;
    
    class Client {
        public:
            Client(std::string ip_address, std::string port);
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

            void HandleSettingChanged(std::string setting);
            
            ClientNetworkManager* net_manager_;

            Settings settings_;
            AudioSystem audio_system_;

            void CharTyped(uint32_t ch);
            void KeyPressed(uint32_t key);
            void KeyReleased(uint32_t key);
            void MouseButtonPressed(int button);
            void MouseButtonReleased(int button);
            void MouseMoved(int screenX, int screenY);

            void Update(float dt);

            void HandleNetworkMessage(packet_t* packet);
            void HandleTicks();

            void Render(Renderer* renderer);

            void TestIntersect(Renderer* renderer, int mx, int my, float* x, float* y);

            void SpawnUnit(unsigned long id);
            void DespawnUnit(unsigned long id);

            Mesh* GetModelForUnit(unsigned long untiId);

            std::vector<game_tick_t> ticks;

            Map* m_map;
            std::list<unit_t> units;

            NavMesh* m_navMesh;

            // Keyboard input
            bool m_keys[0xFF]{ 0 };

            // Mouse input
            bool m_mouseButtons[3]{ 0 };
            int m_mousePos[2]{ 0 };

            int m_sceneWidth = 0;
            int m_sceneHeight = 0;

            int m_mouseClicked[3] = { 0, 0, 0 };
            int m_camDir[2] = { 0, 0 };
            float m_camPos[3] = { 0, 0, 0 };

            int fps;

            std::vector<Mesh*> models;
    };
}
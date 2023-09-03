#pragma once

#include <vector>
#include <list>
#include <string>
#include <map>
#include "client_network_manager.h"
#include "audio_system.h"
#include "settings.h"
#include "pmg_physics.h"
#include "game_object.h"

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
        Physics::Vector2 pos;
        float rot;
        unsigned long unitId;
    } unit_t;
    
    class Client {
        public:
            Client(std::string ip_address, std::string port);
            ~Client();
            void Run();

        private:
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
            
            ClientNetworkManager net_manager_;

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
            void HandleTicks(float dt);

            void Render();
            void RenderGameUI();

            void TestIntersect(Renderer* renderer, int mx, int my, float* x, float* y);

            void SpawnUnit(unsigned long id);
            void SpawnUnit(unsigned long unitId, unsigned long unit_type, Physics::Vector2 pos);
            void DespawnUnit(unsigned long id);

            Mesh* GetModelForUnit(unsigned long untiId);
            GameObject* GetGameObject(unsigned long unit_id);

            Map* m_map;
            std::map<unsigned long, GameObject*> game_objects_;

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
            float m_camPos[3] = { 0, 20.0f, -10.0f };

            int fps;

            unsigned int my_unit_id_;
            bool unit_id_received_ = FALSE;

            unsigned long current_tick_ = 0;
    };
}
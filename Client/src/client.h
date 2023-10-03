#pragma once

#include <vector>
#include <list>
#include <string>
#include <map>
#include "ClientNetworkManager.h"
#include "AudioSystem.h"
#include "Settings.h"
#include "pmg_physics.h"
#include "GameObject.h"
#include "AssetManager.h"

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
        typedef struct {
            unsigned long long received;
            std::vector<uint8_t> data;
        } game_tick_t;

        public:
            Client();
            ~Client();
            void Run(std::string ip_address, std::string port);

            void HandleUnitIdPacket(std::vector<uint8_t> data);
            void HandleGameTickPacket(std::vector<uint8_t> data);
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
            AssetManager assetManager_;

            void CharTyped(uint32_t ch);
            void KeyPressed(uint32_t key);
            void KeyReleased(uint32_t key);
            void MouseButtonPressed(int button);
            void MouseButtonReleased(int button);
            void MouseMoved(int screenX, int screenY);

            void Update(float dt);

            void SimulateTick(game_tick_t& tick, double diff);
            void HandleTicks(float dt);

            void Render();
            void RenderGameUI();

            void TestIntersect(Renderer* renderer, int mx, int my, float* x, float* y);

            void SpawnUnit(UnitId id);
            void SpawnUnit(UnitId unitId, unsigned long unit_type, Team team, Physics::Vector3 pos);
            void DespawnUnit(UnitId id);

            GameObject* GetGameObject(UnitId unit_id);

            Map* m_map;
            std::map<UnitId, GameObject*> game_objects_;

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
            float m_camPos[3] = { 0, 15.0f, -8.0f };
            double last_move = 0;

            int fps;

            UnitId my_unit_id_;
            bool unit_id_received_ = FALSE;

            unsigned long current_tick_ = 0;
            std::vector<game_tick_t> ticks;

            int cooldowns[4] = { -1, -1 , -1 , -1 };
            int total_cooldowns[4] = { -1, -1 , -1 , -1 };
    };
}
#pragma once

#include "IClientState.h"
#include "IClientStateHandler.h"
#include <vector>
#include <map>
#include "pmg_physics.h"
#include "ClientNetworkManager.h"
#include "GameObject.h"
#include "AssetManager.h"
#include "Renderer.h"

namespace PMG {
    class NavMesh;
    class Mesh;
    class Map;

    typedef struct {
        Physics::Vector2 pos;
        float rot;
        unsigned long unitId;
    } unit_t;

	class Game : public IClientState {
        typedef struct {
            unsigned long long received;
            std::vector<uint8_t> data;
        } game_tick_t;
    public:
		Game(std::string server, IClientStateHandler* handler, int width, int height, Renderer* renderer, AssetManager* assetManager);
        ~Game();

        ServerNetworkManager net_manager_;
        Renderer* renderer;
        AssetManager* assetManager_;
        Direct3D* direct3D;

        void HandleUnitIdPacket(std::vector<uint8_t> data);
        void HandleGameTickPacket(std::vector<uint8_t> data);

        void Update(float dt) override;
        void Render(Renderer* renderer) override;

        void RenderGameUI(Renderer* renderer);

        void SimulateTick(game_tick_t& tick, double diff);
        void HandleTicks(float dt);

        void TestIntersect(Renderer* renderer, int mx, int my, float* x, float* y);

        void SpawnUnit(UnitId id);
        void SpawnUnit(UnitId unitId, unsigned long unit_type, Team team, Physics::Vector3 pos);
        void DespawnUnit(UnitId id);

        virtual void CharTyped(uint32_t ch) override;
        virtual void KeyPressed(uint32_t key) override;
        virtual void KeyReleased(uint32_t key) override;
        virtual void MouseButtonPressed(int button) override;
        virtual void MouseButtonReleased(int button) override;
        virtual void MouseMoved(int screenX, int screenY) override;

        GameObject* GetGameObject(UnitId unit_id);

        Map* m_map;
        std::map<UnitId, GameObject*> game_objects_;

        NavMesh* m_navMesh;

        std::vector<game_tick_t> ticks;
        Networking::NetworkHandlerManager<Networking::PacketType> packet_manager;

        UnitId my_unit_id_;
        bool unit_id_received_ = FALSE;

        unsigned long current_tick_ = 0;
        double last_move = 0;

        // Keyboard input
        bool m_keys[0xFF]{ 0 };

        bool m_mouseButtons[3]{ 0 };
        int m_mousePos[2]{ 0 };

        int m_mouseClicked[3] = { 0, 0, 0 };
        int m_camDir[2] = { 0, 0 };
        float m_camPos[3] = { 0, 15.0f, -8.0f };

        int cooldowns[4] = { -1, -1 , -1 , -1 };
        int total_cooldowns[4] = { -1, -1 , -1 , -1 };
	};
}
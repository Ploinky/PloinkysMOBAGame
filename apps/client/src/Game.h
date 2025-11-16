#pragma once

#include "IClientState.h"
#include "IClientStateHandler.h"
#include <vector>
#include <map>
#include <unordered_map>
#include "common/pmg_physics.h"
#include "ClientNetworkManager.h"
#include "GameObject.h"
#include <common/PMG_Common.h>
#include "Renderer.h"
#include <Model.h>
#include "client-asset-manager.h"
#include "common/game/game-state.h"
#include "AudioSystem.h"

class NavMesh;
class NavigationCellGrid;
class Mesh;

typedef struct {
    Vector2 pos;
    float rot;
    uint64_t unitId;
} unit_t;

typedef struct {
    std::string strName;
    HBitmap hIcon;
    EAbilityTargetType eTargetType;
} UIAbility_t;

typedef struct {
    bool bKeyScrollLeft;
    bool bKeyScrollRight;
    bool bKeyScrollUp;
    bool bKeyScrollDown;
    bool bFocusUnit;
} PlayerInput_t;

class Game : public IClientState {
    typedef struct {
        unsigned long long received;
        std::vector<std::vector<uint8_t>> Data;
    } game_tick_t;
public:
	Game(ClientNetworkManager* server, IClientStateHandler* handler, int width, int height);
    ~Game();

    ClientNetworkManager* net_manager_;
    CRenderer* renderer;
    CClientAssetManager* assetManager_;
        
    void AddPacketToCurrentTick(std::vector<uint8_t> data);
    void HandleUnitIdPacket(std::vector<uint8_t> data);
    void HandleGameTickPacket(std::vector<uint8_t> data);
	void HandleUnitMovePacket(std::vector<uint8_t> data);
	void HandleUnitIdlePacket(std::vector<uint8_t> data);
	void HandleUnitSpawnPacket(std::vector<uint8_t> data);
    void HandlePlayParticlePacket(std::vector<uint8_t> data);
    void HandleUnitDespawnPacket(std::vector<uint8_t> data);
    void HandleUnitStatsPacket(std::vector<uint8_t> data);
    void HandleCooldownPacket(std::vector<uint8_t> data);
    void HandleScoreUpdatePacket(std::vector<uint8_t> data);

    void Update(float dt) override;
    void Render(CRenderer* renderer) override;

    void RenderGameUI(CRenderer* renderer);

    void SimulateTick(game_tick_t& tick, double diff);
    void HandleTicks(float dt);

    void TestIntersect(CRenderer* renderer, int mx, int my, float* x, float* y);

    void SpawnUnit(UnitId id);
    void SpawnUnit(UnitId unitId, uint64_t unit_type, Team team, Vector3 pos);
    void DespawnUnit(UnitId id);

    virtual void CharTyped(uint32_t ch) override;
    virtual void KeyPressed(uint32_t key) override;
    virtual void KeyReleased(uint32_t key) override;
    virtual void MouseButtonPressed(int button) override;
    virtual void MouseButtonReleased(int button) override;
    virtual void MouseMoved(int screenX, int screenY) override;

    virtual void Action(EInputAction eAction) override;
    virtual void ActionReleased(EInputAction eAction) override;

    GameObject* GetGameObject(UnitId unit_id);

    std::map<UnitId, GameObject*> game_objects_;
    std::vector<ParticleEffect*> m_vecGlobalParticles;

    CGameState m_gameState;

    NavMesh* m_navMesh;
    NavigationCellGrid* m_navGrid;

    std::vector<game_tick_t> ticks;
    game_tick_t m_receivingTick;
    NetworkHandlerManager<PacketType, std::function<void(std::vector<uint8_t>)>> packet_manager;

    UnitId my_unit_id_;
    bool unit_id_received_ = false;

    float m_fCurrentFrameDelta = 0;
    uint64_t current_tick_ = 0;
    double last_move = 0;

    // Keyboard input
    bool m_keys[0xFF]{ 0 };

    bool m_mouseButtons[3]{ 0 };
    int m_mousePos[2]{ 0 };

    int m_mouseClicked[3] = { 0, 0, 0 };
    int m_camDir[2] = { 0, 0 };
    float m_camPos[3] = { 0, 1500.0f, -800.0f };

    int cooldowns[4] = { -1, -1 , -1 , -1 };
    int total_cooldowns[4] = { -1, -1 , -1 , -1 };

    int m_iTeam1Score;
    int m_iTeam2Score;

    bool m_bGameHasEnded;

    std::vector<std::string> m_vecChat;
    std::vector<UIAbility_t> m_vecAbilities;

    HBitmap m_hGenericIcon = INVALID_ASSET_HANDLE;
    // TODO
    AudioSystem* m_pAudioSystem;
    HSound m_hThunderstrikeSound = INVALID_ASSET_HANDLE;
    HSound m_hStormcallerDeath = INVALID_ASSET_HANDLE;
    HSound m_hStormcallerAttack = INVALID_ASSET_HANDLE;

	GameObject* pObjectUnderCursor = nullptr;
    PlayerInput_t m_playerInput;
};

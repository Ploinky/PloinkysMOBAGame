#pragma once

#include <list>
#include <map>
#include <functional>
#include <string>
#include "common/pmg_networking.h"
#include "Components.h"
#include "IServerState.h"
#include "NetworkManager.h"
#include "LobbyPlayer.h"
#include "GameObject.h"
#include <common/PMG_Common.h>
#include <Buff.h>

#include "systems/NetworkSystem.h"
#include "systems/damage-system.h"
#include "systems/spell-system.h"
#include "systems/movement-system.h"

#include "game/wave-manager.h"

#include <game/server-game-state.h>
#include "game/trigger.h"

class Missile;
static uint64_t current_network_id_ = 0;

class ComponentRegistry;
class NavMesh;
class Character;
class SpellTargetInfo;
class CNavigationSystem;

// Tickrate of the server in ms per tick
// Lower values require a faster server and connection
// Higher values lead to lag...
#define TICKRATE 1000.0f/60.0f

enum UnitActionType {
    SPAWN,
    IDLE,
    MOVE,
    DESPAWN
};

class UnitAction {
public:
    UnitActionType type;
};

enum UnitType {
    PLAYER
};

typedef struct {
    float x;
    float y;
    float tx;
    float ty;
    UnitType type;
    uint64_t unitId;
    UnitAction currentAction;
} unit_t;

class Client : public IServerState {
public:
    Client(IServerStateHandler* handler, ServerNetworkManager* networkManager, AssetManager* assetManager, LobbyPlayer* players[10]);

    NetworkHandlerManager<PacketType, std::function<void(std::vector<uint8_t>)>>* packet_manager;

    void Start();

    void AddPlayerForNetworkId(int index, LobbyPlayer* player);

    void PlayerMoveCommand(PlayerID playerId, float nx, float ny);
    void PlayerStopCommand(PlayerID playerId);
    void PlayerAttackCommand(PlayerID playerId, uint64_t target_id);
    void PlayerCastSpellCommand(PlayerID playerId, int spell_slot, SpellTargetInfo* target_info);

    void SpawnMissile(Missile* missile);


    virtual void Update(float dt) override;
    NavMesh* m_navMesh;
    NavigationMap* m_navMap;

    std::vector<std::vector<uint8_t>> all_ticks;
    CServerGameState GameState;


    void SendMessageToClient(PlayerID playerId, std::vector<uint8_t>* packet) {
        networkManager_->SendToClient(playerId, packet);
    }

    void SendMessageToAllClients(BasePacket& packet) {
        networkManager_->SendToAllClients(packet);
    }

    void SendMessageToAllClients(std::vector<uint8_t>* packet) {
        networkManager_->SendToAllClients(packet);
    }

    void OnMessageReceived(PlayerID playerId, std::vector<uint8_t>* data);

private:

    ServerNetworkManager* networkManager_ = nullptr;
    // CWaveManager m_waveManager;
    // LobbyPlayer* players_[10];
    std::map<PlayerID, LobbyPlayer*> players_;

    std::vector<ISystem*> m_vecSystems;

    CNavigationSystem* m_pNavigationSystem;
    CNetworkSystem* m_pNetworkSystem;
    CDamageSystem m_damageSystem;
    CSpellSystem m_spellSystem;
    CMovementSystem m_moveSystem;
    std::vector<CTrigger> m_vecTriggers;
};
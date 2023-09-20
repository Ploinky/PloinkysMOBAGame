#pragma once 

#include <list>
#include <map>
#include <functional>
#include <string>
#include "networking.h"
#include "pmg_networking.h"
#include "components.h"
#include "game_object.h"
#include "igame_object.h"

namespace PMG {
    class Missile;
    static unsigned long current_network_id_ = 0;

    class ComponentRegistry;
    class NavMesh;
    class Character;

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
        unsigned long unitId;
        UnitAction currentAction;
    } unit_t;

    typedef struct {
        unsigned long unitId;
        unsigned long networkId;
    } player_t;

    class Game {
    public:
        Game();

        std::function<void(unsigned long, std::vector<uint8_t>*)> on_sendToClient;
        std::function<void(std::vector<uint8_t>*)> on_sendToAllClients;

        Networking::NetworkHandlerManager<Networking::PacketType>* packet_manager;

        void Start();

        void AddPlayerForNetworkId(unsigned long netId);
        void RemovePlayerForNetworkId(unsigned long netId);

        void PlayerMoveCommand(unsigned long netId, float nx, float ny);
        void PlayerStopCommand(unsigned long netId);
        void PlayerAttackCommand(unsigned long netId, unsigned long target_id);
        void PlayerCastSpellCommand(unsigned long netId, int spell_slot, SpellTargetInfo* target_info);


        void SendPacket(Networking::BasePacket* packet) {
            tick_packets_.push_back(packet);
        }

        IGameObject* GetGameObjectById(unsigned int id) {
            auto it = this->igame_objects_.find(id);
            
            if (it == igame_objects_.end()) {
                return nullptr;
            }

            return it->second;
        }

        void AddGameObject(IGameObject* game_object);
        void SpawnMissile(Missile* missile);

        void DestroyGameObject(IGameObject* to_destroy);

        void CheckCollision(IGameObject* collider);

        void Update(float dt);
        NavMesh* m_navMesh;
        unsigned long gameTick = 0;
        unsigned long current_entity_id_ = 0;

        std::vector<Networking::BasePacket*> tick_packets_;
        std::vector<std::vector<uint8_t>> all_ticks;
        std::map<unsigned int, IGameObject*> igame_objects_;
    private:

        std::map<unsigned long, player_t> players_;
        float lastTick = 0;
    };
}
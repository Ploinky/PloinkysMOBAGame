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

        std::function<void(unsigned long, packet_t*)> on_sendToClient;
        std::function<void(unsigned long, Networking::BasePacket*)> on_sendNewToClient;
        std::function<void(packet_t*)> on_sendToAllClients;
        std::function<void(std::vector<packet_t>)> on_batchSendToAllClients;

        Networking::NetworkHandlerManager<PacketType>* packet_manager;

        void Start();

        void AddPlayerForNetworkId(unsigned long netId);
        void RemovePlayerForNetworkId(unsigned long netId);

        void PlayerMoveCommand(unsigned long netId, float nx, float ny);
        void PlayerStopCommand(unsigned long netId);
        void PlayerAttackCommand(unsigned long netId, unsigned long target_id);
        void PlayerCastSpellCommand(unsigned long netId, int spell_slot, SpellTargetInfo* target_info);

        template<typename T>
        packet_t CreatePacket(PacketType type, T data) {
            packet_t packet{};
            packet.header.type = type;
            packet << data;
            return packet;
        }

        template<typename T>
        void SendPacket(PacketType type, T data) {
            packet_t packet = CreatePacket(type, data);
            tick_packets.push_back(packet);
        }

        void SendPacket(Networking::BasePacket* packet) {
            tick_base_packets_.push_back(packet);
        }

        GameObject* GetGameObjectById(unsigned int id) {
            auto it = this->game_objects_.find(id);
            
            if (it == game_objects_.end()) {
                return nullptr;
            }

            return it->second;
        }

        void AddGameObject(GameObject* game_object);
        void SpawnMissile(Missile* missile);

        void ApplyDamage(GameObject* target, double damage);
        void Heal(GameObject* target, double heal);

        void DestroyGameObject(GameObject* to_destroy);

        void CheckCollision(GameObject* collider);

        void Update(float dt);
        NavMesh* m_navMesh;
        unsigned long gameTick = 0;
        unsigned long current_entity_id_ = 0;

        std::vector<packet_t> tick_packets;
        std::vector<Networking::BasePacket*> tick_base_packets_;
        std::vector<packet_t> all_ticks;
        std::map<unsigned int, GameObject*> game_objects_;
        std::map<unsigned int, IGameObject*> igame_objects_;
    private:

        std::map<unsigned long, player_t> players_;
        float lastTick = 0;
    };
}
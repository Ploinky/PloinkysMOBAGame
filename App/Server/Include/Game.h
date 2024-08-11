#pragma once 

#include <list>
#include <map>
#include <functional>
#include <string>
#include "Common/pmg_networking.h"
#include "Components.h"
#include "IServerState.h"
#include "NetworkManager.h"
#include "LobbyPlayer.h"
#include "IGameObject.h"
#include <Common/PMG_Common.h>

namespace PMG {
    class Missile;
    static uint64_t current_network_id_ = 0;

    class ComponentRegistry;
    class NavMesh;
    class Character;
    class SpellTargetInfo;

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

        Networking::NetworkHandlerManager<Networking::PacketType, std::function<void(std::vector<uint8_t>)>>* packet_manager;

        void Start();

        void AddPlayerForNetworkId(int index, LobbyPlayer* player);

        void PlayerMoveCommand(PlayerID playerId, float nx, float ny);
        void PlayerStopCommand(PlayerID playerId);
        void PlayerAttackCommand(PlayerID playerId, uint64_t target_id);
        void PlayerCastSpellCommand(PlayerID playerId, int spell_slot, SpellTargetInfo* target_info);

        IGameObject* GetGameObjectById(unsigned int id) {
            auto it = this->igame_objects_.find(id);
            
            if (it == igame_objects_.end()) {
                return nullptr;
            }

            return it->second;
        }

        void AddGameObject(IGameObject* game_object);
        void SpawnMissile(Missile* missile);

        void CheckCollision(IGameObject* collider);

        virtual void Update(float dt) override;
		NavigationCellGrid* m_navGrid;
        NavMesh* m_navMesh;
        uint64_t gameTick = 0;
        uint64_t current_entity_id_ = 0;

        std::vector<std::vector<uint8_t>> all_ticks;
        std::map<unsigned int, IGameObject*> igame_objects_;


        void SendMessageToClient(PlayerID playerId, std::vector<uint8_t>* packet) {
            networkManager_->SendToClient(playerId, packet);
        }

        void SendMessageToAllClients(std::vector<uint8_t>* packet) {
            networkManager_->SendToAllClients(packet);
        }

        void OnMessageReceived(PlayerID playerId, std::vector<uint8_t>* data);

    private:

        // std::map<uint64_t, player_t> players_;
        float lastTick = 0;

        ServerNetworkManager* networkManager_ = nullptr;
        // LobbyPlayer* players_[10];
        std::map<PlayerID, LobbyPlayer*> players_;
    };
}
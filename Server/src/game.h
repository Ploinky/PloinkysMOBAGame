#pragma once 

#include <list>
#include <functional>
#include <string>
#include "networking.h"

namespace PMG {
    class ComponentRegistry;
    class NavMesh;

    // Tickrate of the server in ms per tick
    // Lower values require a faster server and connection
    // Higher values lead to lag...
#define TICKRATE 1000/60

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
        std::function<void(packet_t*)> on_sendToAllClients;
        std::function<void(std::vector<packet_t>)> on_batchSendToAllClients;

        void Start();

        void AddPlayerForNetworkId(unsigned long netId);
        void RemovePlayerForNetworkId(unsigned long netId);

        void PlayerMoveCommand(unsigned long netId, float nx, float ny);

        void Update(float dt);
    private:
        ComponentRegistry* m_componentRegistry;
        NavMesh* m_navMesh;

        std::list<player_t> players;
        float lastTick = 0;
        unsigned long gameTick = 0;
    };
}
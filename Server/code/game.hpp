#pragma once 

#include <list>
#include <limits>
#include <functional>
#include <string>

namespace PMG {
    class ComponentRegistry;
    class NavMesh;

    // Tickrate of the server in ms per tick
    // Lower values require a faster server and connection
    // Higher values lead to lag...
#define TICKRATE 16.66

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

        std::function<void(std::string)> on_newGameTick;
        std::function<void(unsigned long, std::string)> on_sendToClient;

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
        unsigned long gameTick = std::numeric_limits<unsigned long>::max();
    };
}
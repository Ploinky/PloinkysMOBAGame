#include "game.h"
#include "component_registry.h"
#include "components.h"
#include "navigation.h"
#include "pmg_physics.h"
#include "networking.h"

namespace PMG {
    unsigned long g_unitId = 0;

    Game::Game() {
        m_componentRegistry = new ComponentRegistry();
        m_navMesh = new NavMesh();
        m_navMesh->LoadFromFile("map1");
    }

    template<typename T>
    packet_t CreatePacket(PacketType type, T data) {
        packet_t packet{};
        packet.header.type = type;
        packet << data;
        return packet;
    }

    void Game::AddPlayerForNetworkId(unsigned long netId) {

        for (auto ent : m_componentRegistry->GetEntities<transform_t>()) {
            transform_t* t = m_componentRegistry->GetComponent<transform_t>(ent);
            if (t) {
                packet_t packet = CreatePacket<pck_unit_spawn>(PacketType::UNITSPAWN, {ent, t->x, t->y});
                on_sendToClient(netId, &packet);
            }
        }

        entity_id id = m_componentRegistry->Create();
        m_componentRegistry->AddComponent<transform_t>(id, { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f });
        m_componentRegistry->AddComponent<network_t>(id, { netId });
        m_componentRegistry->AddComponent<spawn_t>(id, { 0.0f, 0.0f });
        m_componentRegistry->AddComponent<nav_agent_t>(id, { {}, 0.0f });
        m_componentRegistry->AddComponent<stats_t>(id, { 100, 100 });

        packet_t packet = CreatePacket<pck_client_unit_id>(PacketType::PCK_CLIENT_UNIT_ID, { id });
        on_sendToClient(netId, &packet);
    }

    void Game::RemovePlayerForNetworkId(unsigned long netId) {
        for (auto ent : m_componentRegistry->GetEntities<network_t>()) {
            if (m_componentRegistry->GetComponent<network_t>(ent)->netId == netId) {
                m_componentRegistry->AddComponent<despawn_t>(ent);
            }
        }
    }

    void Game::PlayerMoveCommand(unsigned long netId, float nx, float ny) {
        for (auto ent : m_componentRegistry->GetEntities<network_t>()) {
            if (m_componentRegistry->GetComponent<network_t>(ent)->netId == netId) {
                nav_agent_t* agent = m_componentRegistry->GetComponent<nav_agent_t>(ent);
                agent->target.x = nx;
                agent->target.z = ny;
                // Clear old path!
                agent->path.clear();
            }
        }
    }

    void Game::PlayerStopCommand(unsigned long netId) {
        for (auto ent : m_componentRegistry->GetEntities<network_t>()) {
            if (m_componentRegistry->GetComponent<network_t>(ent)->netId == netId) {
                nav_agent_t* agent = m_componentRegistry->GetComponent<nav_agent_t>(ent);
                transform_t* transform = m_componentRegistry->GetComponent<transform_t>(ent);
                agent->target.x = transform->x;
                agent->target.z = transform->y;
                transform->tx = transform->x;
                transform->ty = transform->y;
                // Clear old path!
                agent->path.clear();
            }
        }
    }

    void Game::PlayerAttackCommand(unsigned long netId, unsigned long target_id) {
        stats_t* target_unit_stats = m_componentRegistry->GetComponent<stats_t>(target_id);

        target_unit_stats->health -= 1;

        if (target_unit_stats->health < 0) {
            target_unit_stats->health = 0;
        }
    }

    void Game::Start() {

    }

    void Game::Update(float dt) {
        lastTick += dt;

        // ca. 16 ms per tick
        if (lastTick < TICKRATE / 1000.0f) {
            return;
        }

        // Next gametick -> wrap to zero at start. Yikes...
        gameTick++;
        lastTick -= TICKRATE / 1000.0f;

        // ======== Navigation system ========
        for (auto ent : m_componentRegistry->GetEntities<nav_agent_t>()) {
            transform_t* transform = m_componentRegistry->GetComponent<transform_t>(ent);

            // What to do with entities that have nav agents but no transforms? Makes no sense.
            if (!transform) {
                continue;
            }

            nav_agent_t* navAgent = m_componentRegistry->GetComponent<nav_agent_t>(ent);

            if (navAgent->target.x == transform->x && navAgent->target.z == transform->y) {
                // Already at target
                continue;
            }

            if (navAgent->path.empty()) {
                // No path to follow, we need a new path!
                navAgent->path = m_navMesh->PlanPath({ transform->x, 0, transform->y }, navAgent->target);

                // New path is empty, we are requesting an invalid path
                if (navAgent->path.empty()) {
                    continue;
                }
                // First is our start point? yikes.
                navAgent->path.pop_front();
            }

            vertex_t intermediateTarget = navAgent->path.front();

            if (abs(transform->x - intermediateTarget.x) < 0.001 && abs(transform->y - intermediateTarget.z) < 0.001) {
                // Next frame we follow next?!
                navAgent->path.pop_front();
                intermediateTarget = navAgent->path.front();
            }

            if (transform->tx != intermediateTarget.x) {
              transform->tx = intermediateTarget.x;
            }

            if (transform->ty != intermediateTarget.z) {
              transform->ty = intermediateTarget.z;
            }
        }


        // ======== Movement system ========
        for (auto ent : m_componentRegistry->GetEntities<transform_t>()) {
            transform_t* transform = m_componentRegistry->GetComponent<transform_t>(ent);
            if (Physics::CompareDouble(transform->x, transform->tx) && Physics::CompareDouble(transform->y, transform->ty)) {
                continue;
            }

            float dx = transform->tx - transform->x;
            float dy = transform->ty - transform->y;
            float length = sqrt(dx * dx + dy * dy);


            dx /= length;
            dy /= length;

            float newX = transform->x + 6.0f * dx * TICKRATE / 1000.0f;
            float newY = transform->y + 6.0f * dy * TICKRATE / 1000.0f;

            transform->x = (transform->x < transform->tx && newX >= transform->tx) || (transform->x > transform->tx && newX <= transform->tx) ? transform->tx : newX;
            transform->y = (transform->y < transform->ty && newY >= transform->ty) || (transform->y > transform->ty && newY <= transform->ty) ? transform->ty : newY;

            if (transform->x != transform->tx || transform->y != transform->ty) {
                transform->r = -atan2(transform->ty - transform->y, transform->tx - transform->x) * 180.0f / M_PI;
            }
        }

        // ======== Networking system ========
        packet_t tickPacket{};
        tickPacket.header.type = PacketType::GAME_TICK;

        for (auto ent : m_componentRegistry->GetEntities<network_t>()) {
            spawn_t* spawnT = m_componentRegistry->GetComponent<spawn_t>(ent);
            if (spawnT) {
                packet_t packet = CreatePacket<pck_unit_spawn_t>(PacketType::UNITSPAWN, { ent, spawnT->x, spawnT->y });
                tickPacket << packet;

                m_componentRegistry->RemoveComponent<spawn_t>(ent);
            }

            transform_t* t = m_componentRegistry->GetComponent<transform_t>(ent);
            if (t) {
                packet_t packet = CreatePacket<pck_unit_move_t>(PacketType::UNITMOVE, { ent, t->x, t->y, t->r });
                tickPacket << packet;
            }

            stats_t* stats = m_componentRegistry->GetComponent<stats_t>(ent);
            if (stats) {
                packet_t packet = CreatePacket<pck_unit_stats_t>(PacketType::PCK_STATS, { ent, stats->health, stats->max_health });
                tickPacket << packet;
            }

            despawn_t* despawn = m_componentRegistry->GetComponent<despawn_t>(ent);
            if (despawn) {
                packet_t packet = CreatePacket<pck_unit_despawn_t>(PacketType::UNITDESPAWN, { ent });
                tickPacket << packet;

                m_componentRegistry->Destroy(ent);
            }
        }

        tickPacket << gameTick;
        tickPacket.header.size = tickPacket.size();

        on_sendToAllClients(&tickPacket);
    }
}
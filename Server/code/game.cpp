#include "game.hpp"
#include "component_registry.hpp"
#include "components.hpp"
#include "Shared/code/navigation.hpp"

namespace PMG {
    unsigned long g_unitId = 0;

    Game::Game() {
        m_componentRegistry = new ComponentRegistry();
        m_navMesh = new NavMesh();
        m_navMesh->LoadFromFile("map1");
    }
    void Game::AddPlayerForNetworkId(unsigned long netId) {
        std::string msg("SpawnUnits");

        for (auto ent : m_componentRegistry->GetEntities<transform_t>()) {
            transform_t* t = m_componentRegistry->GetComponent<transform_t>(ent);
            if (t) {
                msg.append("|");
                msg.append(std::to_string(ent));
                msg.append(";");
                msg.append(std::to_string(t->x));
                msg.append(";");
                msg.append(std::to_string(t->y));
            }
        }


        on_sendToClient(netId, msg);

        entity_id id = m_componentRegistry->Create();
        m_componentRegistry->AddComponent<transform_t>(id, { 0.0f, 0.0f });
        m_componentRegistry->AddComponent<network_t>(id, { netId });
        m_componentRegistry->AddComponent<spawn_t>(id, { 0.0f, 0.0f });
        m_componentRegistry->AddComponent<nav_agent_t>(id, { {}, 0.0f });
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
            if (transform->x != transform->tx || transform->y != transform->ty) {
                float dx = transform->tx - transform->x;
                float dy = transform->ty - transform->y;
                float length = sqrt(dx * dx + dy * dy);

                dx /= length;
                dy /= length;

                float newX = transform->x + 6.0f * dx * TICKRATE / 1000.0f;
                float newY = transform->y + 6.0f * dy * TICKRATE / 1000.0f;

                transform->x = (transform->x < transform->tx&& newX >= transform->tx) || (transform->x > transform->tx && newX <= transform->tx) ? transform->tx : newX;
                transform->y = (transform->y < transform->ty&& newY >= transform->ty) || (transform->y > transform->ty && newY <= transform->ty) ? transform->ty : newY;
            }
        }

        // ======== Networking system ========
        std::string msg = std::string("Tick|")
            .append(std::to_string(gameTick));

        for (auto ent : m_componentRegistry->GetEntities<network_t>()) {
            spawn_t* spawnT = m_componentRegistry->GetComponent<spawn_t>(ent);
            if (spawnT) {
                msg.append("|UnitSpawn;")
                    .append(std::to_string(ent))
                    .append(";")
                    .append(std::to_string(spawnT->x))
                    .append(";")
                    .append(std::to_string(spawnT->y));

                m_componentRegistry->RemoveComponent<spawn_t>(ent);
            }

            transform_t* t = m_componentRegistry->GetComponent<transform_t>(ent);
            if (t) {
                msg.append("|UnitMove;")
                    .append(std::to_string(ent))
                    .append(";")
                    .append(std::to_string(t->x))
                    .append(";")
                    .append(std::to_string(t->y));
            }

            despawn_t* despawn = m_componentRegistry->GetComponent<despawn_t>(ent);
            if (despawn) {
                msg.append("|UnitDespawn;")
                    .append(std::to_string(ent));
                m_componentRegistry->Destroy(ent);
            }
        }

        on_newGameTick(msg);
    }
}
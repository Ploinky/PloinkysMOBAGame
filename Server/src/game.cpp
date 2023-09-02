#include "game.h"
#include "components.h"
#include "navigation.h"
#include "pmg_physics.h"
#include "networking.h"

namespace PMG {
    unsigned long g_unitId = 0;

    Game::Game() {
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

        for (auto go_it : game_objects_) {
            GameObject* go = go_it.second;
            packet_t packet = CreatePacket<pck_unit_spawn>(PacketType::UNITSPAWN, { go->unit_id, go->position.x, go->position.y });
            on_sendToClient(netId, &packet);
        }

        entity_id id = current_entity_id_++;

        packet_t packet = CreatePacket<pck_client_unit_id>(PacketType::PCK_CLIENT_UNIT_ID, { id });
        on_sendToClient(netId, &packet);

        GameObject* game_object = new GameObject();
        game_object->current_action = nullptr;
        game_object->unit_id = id;
        game_objects_.emplace(id, game_object);

        player_t player{};
        player.networkId = netId;
        player.unitId = id;
        players_.emplace(netId, player);

        packet = CreatePacket<pck_unit_spawn_t>(PacketType::UNITSPAWN, { id, 0, 0 });
        on_sendToAllClients(&packet);

        packet = CreatePacket<pck_unit_stats_t>(PacketType::PCK_STATS, { id, 100, 100 });
        on_sendToAllClients(&packet);
    }

    void Game::RemovePlayerForNetworkId(unsigned long netId) {
        packet_t packet = CreatePacket<pck_unit_despawn_t>(PacketType::UNITDESPAWN, { players_.find(netId)->second.unitId });
        on_sendToAllClients(&packet);

        GameObject* go = game_objects_.find(players_.find(netId)->second.unitId)->second;
        game_objects_.erase(go->unit_id);
        delete go;
    }

    void Game::PlayerMoveCommand(unsigned long netId, float nx, float ny) {
        game_objects_.find(players_.find(netId)->second.unitId)->second->current_action = new GameObjectActionMove({ nx, ny, 0 });
    }

    void Game::PlayerStopCommand(unsigned long netId) {

        game_objects_.find(players_.find(netId)->second.unitId)->second->current_action = new GameObjectActionStop();
    }

    void Game::PlayerAttackCommand(unsigned long netId, unsigned long target_id) {
        game_objects_.find(players_.find(netId)->second.unitId)->second->current_action = new GameObjectActionAttackUnit(target_id);
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

        for (auto go_it : game_objects_) {
            GameObject* go = go_it.second;

            if (go->current_action != nullptr) {
                switch (go->current_action->type) {
                case GameObjectActionType::STOP: {
                    go->basic_attack_info.attack_started = FALSE;
                    break;
                }
                case GameObjectActionType::ATTACK_UNIT: {
                    GameObjectActionAttackUnit* action = (GameObjectActionAttackUnit*)go->current_action;
                    GameObject* target = game_objects_.find(action->target_net_id)->second;
                    
                    if (target == nullptr) {
                        // nothing to attack?
                        break;
                    }

                    if ((target->position - go->position).Length() > go->basic_attack_info.range) {
                        // move towards target?
                        break;
                    }

                    // we're in range, check if we can attack
                    unsigned long long ticks_since = gameTick - go->basic_attack_info.last_attack;

                    // how many ms do we wait after 1 attack
                    double ms_per_attack = 1000.0 / go->basic_attack_info.attack_speed;

                    if (ticks_since * TICKRATE < ms_per_attack) {
                        // cannot attack again yet
                        break;
                    }

                    // we can attack, wtf to do now?!
                    // consider forward- and backswing as well, yikes
                    if (!go->basic_attack_info.attack_started) {
                        go->basic_attack_info.attack_started_at = gameTick;
                        go->basic_attack_info.attack_started = TRUE;
                        // ok we start... do we also need to let someone know? :O
                        break;
                    }

                    ticks_since = gameTick - go->basic_attack_info.attack_started_at;
                    double ms_until_hit = ms_per_attack * go->basic_attack_info.hit_point;

                    if (ticks_since * TICKRATE < ms_until_hit) {
                        // still swinging!
                        break;
                    }

                    // attack hits!
                    target->stats.health -= go->basic_attack_info.damage;

                    if (target->stats.health < 0) {
                        target->stats.health = 0;
                    }

                    go->basic_attack_info.last_attack = gameTick;
                    go->basic_attack_info.attack_started = FALSE;

                    packet_t packet = CreatePacket<pck_unit_stats_t>(PacketType::PCK_STATS, { go->unit_id, go->stats.health, go->stats.max_health });
                    on_sendToAllClients(&packet);
                    break;
                }
                case GameObjectActionType::MOVE: {
                    go->basic_attack_info.attack_started = FALSE;
                    GameObjectActionMove* action = (GameObjectActionMove*)go->current_action;

                    // figure out if we're already going to target

                    // ======== Navigation system ========
                    nav_agent_t navAgent = go->nav_agent;
                    navAgent.target.x = action->target_point.x;
                    navAgent.target.z = action->target_point.y;

                    if (navAgent.target.x == go->position.x && navAgent.target.z == go->position.y) {
                        // Already at target
                        continue;
                    }

                    if (navAgent.path.empty()) {
                        // No path to follow, we need a new path!
                        navAgent.path = m_navMesh->PlanPath({ static_cast<float>(go->position.x), 0, static_cast<float>(go->position.y) }, navAgent.target);

                        // New path is empty, we are requesting an invalid path
                        if (navAgent.path.empty()) {
                            continue;
                        }
                        // First is our start point? yikes.
                        navAgent.path.pop_front();
                    }

                    vertex_t intermediateTarget = navAgent.path.front();

                    if (abs(go->position.x - intermediateTarget.x) < 0.001 && abs(go->position.y - intermediateTarget.z) < 0.001) {
                        // Next frame we follow next?!
                        navAgent.path.pop_front();

                        if (!navAgent.path.empty()) {
                            intermediateTarget = navAgent.path.front();
                        }
                    }

                    float tx = intermediateTarget.x;
                    float ty = intermediateTarget.z;

                    if (Physics::CompareDouble(go->position.x, tx) && Physics::CompareDouble(go->position.y, ty)) {
                        continue;
                    }

                    float dx = tx - go->position.x;
                    float dy = ty - go->position.y;
                    float length = sqrt(dx * dx + dy * dy);


                    dx /= length;
                    dy /= length;

                    float newX = go->position.x + 6.0f * dx * TICKRATE / 1000.0f;
                    float newY = go->position.y + 6.0f * dy * TICKRATE / 1000.0f;

                    go->position.x = (go->position.x < tx && newX >= tx) || (go->position.x > tx && newX <= tx) ? tx : newX;
                    go->position.y = (go->position.y < ty && newY >= ty) || (go->position.y > ty && newY <= ty) ? ty : newY;

                    if (go->position.x != tx || go->position.y != ty) {
                        go->rotation.y = -atan2(ty - go->position.y, tx - go->position.x) * 180.0f / M_PI;
                    }

                    packet_t packet = CreatePacket<pck_unit_move_t>(PacketType::UNITMOVE, { go->unit_id, static_cast<float>(go->position.x), static_cast<float>(go->position.y), static_cast<float>(go->rotation.y) });
                    on_sendToAllClients(&packet);
                    break;
                }
                }
            }
        }
    }
}
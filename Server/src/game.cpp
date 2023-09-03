#include "game.h"
#include "components.h"
#include "navigation.h"
#include "pmg_physics.h"
#include "networking.h"
#include "character.h"

namespace PMG {
    unsigned long g_unitId = 0;

    Game::Game() {
        m_navMesh = new NavMesh();
        m_navMesh->LoadFromFile("map1");
    }

    void Game::AddPlayerForNetworkId(unsigned long netId) {
        for (auto go_it : game_objects_) {
            Character* go = (Character*) go_it.second;
            packet_t packet = CreatePacket<pck_unit_spawn>(PacketType::UNITSPAWN, { go->unit_id, 0, go->team, go->position.x, go->position.z });
            on_sendToClient(netId, &packet);

            packet = CreatePacket<pck_unit_stats_t>(PacketType::PCK_STATS, { go->unit_id, go->stats.health, go->stats.max_health});
            on_sendToAllClients(&packet);
        }

        entity_id id = current_entity_id_++;

        packet_t packet = CreatePacket<pck_client_unit_id>(PacketType::PCK_CLIENT_UNIT_ID, { id });
        on_sendToClient(netId, &packet);

        Character* game_object = new Character();
        game_object->current_action = nullptr;
        game_object->unit_id = id;

        if (players_.size() % 2 == 0) {
            game_object->team = Team::TEAM_1;
        }
        else {
            game_object->team = Team::TEAM_2;
        }

        game_objects_.emplace(id, game_object);

        player_t player{};
        player.networkId = netId;
        player.unitId = id;
        players_.emplace(netId, player);

        packet = CreatePacket<pck_unit_spawn_t>(PacketType::UNITSPAWN, { game_object->unit_id, 0, game_object->team, 0, 0 });
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

    void Game::AddGameObject(GameObject* game_object) {
        game_objects_.emplace(game_object->unit_id, game_object);
    }

    void Game::SpawnMissile(Character* missile) {
        game_objects_.emplace(missile->unit_id, missile);

        SendPacket<pck_unit_spawn_t>(PacketType::UNITSPAWN, { missile->unit_id, 1, missile->team, missile->position.x, missile->position.y });
    }

    void Game::PlayerMoveCommand(unsigned long netId, float nx, float ny) {
        ((Character*) game_objects_.find(players_.find(netId)->second.unitId)->second)->current_action = new GameObjectActionMove({ nx, ny, 0 });
    }

    void Game::PlayerStopCommand(unsigned long netId) {

        ((Character*) game_objects_.find(players_.find(netId)->second.unitId)->second)->current_action = new GameObjectActionStop();
    }

    void Game::PlayerAttackCommand(unsigned long netId, unsigned long target_id) {
        Character* actor = (Character*)game_objects_.find(players_.find(netId)->second.unitId)->second;
        Character* target = (Character*)GetGameObjectById(target_id);

        if (target == nullptr || target->unit_id == actor->unit_id || target->team == actor->team) {
            // nothing to attack?
            actor->current_action = new GameObjectActionStop();
        }
        else {
            actor->current_action = new GameObjectActionAttackUnit(target_id);
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

        packet_t packet = CreatePacket<pck_tick_t>(PacketType::GAME_TICK, { gameTick });
        on_sendToAllClients(&packet);

        for (auto go_it : game_objects_) {
            GameObject* go = go_it.second;
            go->Think(dt, this);
        }

        std::erase_if(game_objects_, [](auto& kv) { return kv.second->is_destroyed; });
    }
}
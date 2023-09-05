#include "game.h"
#include "components.h"
#include "navigation.h"
#include "pmg_physics.h"
#include "networking.h"
#include "game_object.h"
#include "missile.h"
#include "building.h"
#include "football_person.h"

namespace PMG {
    unsigned long g_unitId = 0;

    Game::Game() {
        m_navMesh = new NavMesh();
        m_navMesh->LoadFromFile("map1");
    }

    void Game::AddPlayerForNetworkId(unsigned long netId) {
        for (auto tick : all_ticks) {
            on_sendToClient(netId, &tick);
        }

        entity_id id = current_entity_id_++;

        packet_t packet = CreatePacket<pck_client_unit_id>(PacketType::PCK_CLIENT_UNIT_ID, { id });
        on_sendToClient(netId, &packet);

        FootballPerson* game_object = new FootballPerson();
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

        SendPacket<pck_unit_spawn_t>(PacketType::UNITSPAWN, { game_object->unit_id, 0, game_object->team, 0, 0 });
        SendPacket<pck_unit_stats_t>(PacketType::PCK_STATS, { id, 100, 100 });
    }

    void Game::RemovePlayerForNetworkId(unsigned long netId) {
        SendPacket<pck_unit_despawn_t>(PacketType::UNITDESPAWN, { players_.find(netId)->second.unitId });

        GameObject* go = game_objects_.find(players_.find(netId)->second.unitId)->second;
        game_objects_.erase(go->unit_id);
        delete go;
    }

    void Game::AddGameObject(GameObject* game_object) {
        game_object->unit_id = current_entity_id_++;
        game_objects_.emplace(game_object->unit_id, game_object);

        }

    void Game::SpawnMissile(Missile* missile) {
        // adjust target point here???
        Physics::Vector3 dir = missile->target_point - missile->position;
        dir = dir.Normalize();

        dir = dir * missile->max_distance;

        missile->target_point = missile->position + dir;

        missile->unit_id = current_entity_id_++;
        game_objects_.emplace(missile->unit_id, missile);

        SendPacket<pck_unit_spawn_t>(PacketType::UNITSPAWN, { missile->unit_id, 1, missile->team, missile->position.x, missile->position.y });
    }

    void Game::PlayerMoveCommand(unsigned long netId, float nx, float ny) {
        game_objects_.find(players_.find(netId)->second.unitId)->second->current_action = new GameObjectActionMove({ nx, ny, 0 });
    }

    void Game::PlayerStopCommand(unsigned long netId) {

        game_objects_.find(players_.find(netId)->second.unitId)->second->current_action = new GameObjectActionStop();
    }

    void Game::PlayerAttackCommand(unsigned long netId, unsigned long target_id) {
        GameObject* actor = game_objects_.find(players_.find(netId)->second.unitId)->second;
        GameObject* target = GetGameObjectById(target_id);

        if (target == nullptr || target->unit_id == actor->unit_id || target->team == actor->team) {
            // nothing to attack?
            // maybe follow if it is a friend?
            actor->current_action = new GameObjectActionStop();
        }
        else {
            actor->current_action = new GameObjectActionAttackUnit(target_id);
        }
    }

    void Game::PlayerCastSpellCommand(unsigned long netId, int spell_slot, Physics::Vector3 target_point) {
        GameObject* actor = GetGameObjectById(players_.find(netId)->second.unitId);

        if (actor->spells[spell_slot]->remaining_cooldown != -1) {
            // nope!
            return;
        }

        GameObjectActionCastSpell* new_action = new GameObjectActionCastSpell(spell_slot);
        new_action->target_point = target_point;
        actor->current_action = new_action;
    }

    void Game::Start() {
        Building* tower = new Building();
        tower->position = { 10, 0, 0 };
        tower->team = Team::TEAM_2;
        AddGameObject(tower);
        SendPacket<pck_unit_spawn_t>(PacketType::UNITSPAWN, { tower->unit_id, 2, tower->team, tower->position.x, tower->position.y });


        Building* tower2 = new Building();
        tower2->position = { -10, 0, 0 };
        tower2->team = Team::TEAM_1;
        AddGameObject(tower2);
        SendPacket<pck_unit_spawn_t>(PacketType::UNITSPAWN, { tower2->unit_id, 2, tower2->team, tower2->position.x, tower2->position.y });
    }

    void Game::ApplyDamage(GameObject* target, double damage) {
        if (!target->IsTargetable()) {
            return;
        }

        target->stats.health -= damage;

        if (target->stats.health < 0) {
            target->stats.health = 0;
            // do something?
        }

        SendPacket<pck_unit_stats_t>(PacketType::PCK_STATS, { target->unit_id, target->stats.health, target->stats.max_health });
    }

    void Game::DestroyGameObject(GameObject* to_destroy) {
        SendPacket<pck_unit_despawn_t>(PacketType::UNITDESPAWN, { to_destroy->unit_id });
        to_destroy->is_destroyed = true;
    }

    void Game::Update(float dt) {
        lastTick += dt;

        // ca. 16 ms per tick
        if (lastTick < TICKRATE / 1000.0f) {
            return;
        }

        // Next gametick -> wrap to zero at start. Yikes...
        lastTick -= TICKRATE / 1000.0f;

        for (auto go_it : game_objects_) {
            GameObject* go = go_it.second;
            go->controller->Think(this, go);
            go->Think(dt, this);
        }

        packet_t packet{};
        packet.header.type = PacketType::GAME_TICK;

        for (packet_t pack : tick_packets) {
            packet << pack;
        }

        packet << gameTick++;
        all_ticks.push_back(packet);

        on_sendToAllClients(&packet);

        tick_packets.clear();

        std::erase_if(game_objects_, [](auto& kv) { return kv.second->is_destroyed; });
    }
}
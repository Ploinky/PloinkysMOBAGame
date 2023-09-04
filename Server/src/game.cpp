#include "game.h"
#include "components.h"
#include "navigation.h"
#include "pmg_physics.h"
#include "networking.h"
#include "character.h"
#include "missile.h"

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
        game_objects_.emplace(game_object->unit_id, game_object);
    }

    void Game::SpawnMissile(Missile* missile) {
        missile->unit_id = current_entity_id_++;
        game_objects_.emplace(missile->unit_id, missile);

        SendPacket<pck_unit_spawn_t>(PacketType::UNITSPAWN, { missile->unit_id, 1, missile->team, missile->position.x, missile->position.y });
    }

    void Game::PlayerMoveCommand(unsigned long netId, float nx, float ny) {
        ((Character*) game_objects_.find(players_.find(netId)->second.unitId)->second)->current_action = new CharacterActionMove({ nx, ny, 0 });
    }

    void Game::PlayerStopCommand(unsigned long netId) {

        ((Character*) game_objects_.find(players_.find(netId)->second.unitId)->second)->current_action = new CharacterActionStop();
    }

    void Game::PlayerAttackCommand(unsigned long netId, unsigned long target_id) {
        Character* actor = (Character*)game_objects_.find(players_.find(netId)->second.unitId)->second;
        Character* target = (Character*)GetGameObjectById(target_id);

        if (target == nullptr || target->unit_id == actor->unit_id || target->team == actor->team) {
            // nothing to attack?
            // maybe follow if it is a friend?
            actor->current_action = new CharacterActionStop();
        }
        else {
            actor->current_action = new CharacterActionAttackUnit(target_id);
        }
    }

    void Game::PlayerCastSpellCommand(unsigned long netId, int spell_slot, Physics::Vector3 target_point) {
        Character* actor = (Character*) GetGameObjectById(players_.find(netId)->second.unitId);

        if (actor->spells[spell_slot]->remaining_cooldown != -1) {
            // nope!
            return;
        }

        CharacterActionCastSpell* new_action = new CharacterActionCastSpell(spell_slot);
        new_action->target_point = target_point;
        actor->current_action = new_action;
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
        lastTick -= TICKRATE / 1000.0f;

        for (auto go_it : game_objects_) {
            GameObject* go = go_it.second;
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
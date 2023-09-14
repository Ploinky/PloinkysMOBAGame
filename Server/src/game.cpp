#include "game.h"
#include "components.h"
#include "navigation.h"
#include "pmg_physics.h"
#include "networking.h"
#include "game_object.h"
#include "missile.h"
#include "building.h"
#include "football_person.h"

#include "minion_spawner.h"
#include "pmg_networking.h"

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

        packet_manager = new Networking::NetworkHandlerManager<PacketType>();

        entity_id id = current_entity_id_++;


        Networking::UnitIdPacket packet = Networking::UnitIdPacket();
        packet.unit_id = id;
        on_sendNewToClient(netId, &packet);

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

        SendPacket<pck_unit_spawn_t>(PacketType::UNITSPAWN, { game_object->unit_id, UnitPrefab::FOOTBALL_PERSON, game_object->team, 0, 0 });

        Networking::UnitStatsPacket* stats = new Networking::UnitStatsPacket();
        stats->unit = id;
        stats->health = 100;
        stats->max_health = 100;

        SendPacket(stats);
    }

    void Game::RemovePlayerForNetworkId(unsigned long netId) {
        Networking::DespawnPacket* despawn = new Networking::DespawnPacket();
        despawn->unit = players_.find(netId)->second.unitId;
        SendPacket(despawn);

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

        SendPacket<pck_unit_spawn_t>(PacketType::UNITSPAWN, { missile->unit_id, UnitPrefab::THROW_FOOTBALL, missile->team, missile->position.x, missile->position.y });
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
        actor->current_action = new GameObjectActionAttackUnit(target_id);
    }

    void Game::PlayerCastSpellCommand(unsigned long netId, int spell_slot, SpellTargetInfo* target_info) {
        GameObject* actor = GetGameObjectById(players_.find(netId)->second.unitId);

        if (actor->spells[spell_slot]->remaining_cooldown != -1) {
            // nope!
            return;
        }

        GameObjectActionCastSpell* new_action = new GameObjectActionCastSpell(spell_slot);
        new_action->target_info = target_info;
        actor->current_action = new_action;
    }

    void Game::Start() {
        Building* tower = new Building();
        tower->position = { 10, 0, 0 };
        tower->team = Team::TEAM_2;
        AddGameObject(tower);
        SendPacket<pck_unit_spawn_t>(PacketType::UNITSPAWN, { tower->unit_id, UnitPrefab::TOWER, tower->team, tower->position.x, tower->position.y });


        Building* tower2 = new Building();
        tower2->position = { -10, 0, 0 };
        tower2->team = Team::TEAM_1;
        AddGameObject(tower2);
        SendPacket<pck_unit_spawn_t>(PacketType::UNITSPAWN, { tower2->unit_id, UnitPrefab::TOWER, tower2->team, tower2->position.x, tower2->position.y });

        MinionSpawner* minion_spawner = new MinionSpawner();
        igame_objects_.emplace(0, minion_spawner);
    }

    void Game::ApplyDamage(GameObject* target, double damage) {
        if (!target->IsTargetable()) {
            return;
        }

        target->stats.health -= damage;

        if (target->stats.health < 0) {
            target->stats.health = 0;
            // TODO do something?
        }

    }

    void Game::Heal(GameObject* target, double heal) {
        if (!target->IsTargetable()) {
            return;
        }

        target->stats.health += heal;

        if (target->stats.health > target->stats.max_health) {
            target->stats.health = target->stats.max_health;
            // do something?
        }

        Networking::UnitStatsPacket* stats = new Networking::UnitStatsPacket();
        stats->unit = target->unit_id;
        stats->health = target->stats.health;
        stats->max_health = target->stats.max_health;
        SendPacket(stats);
    }

    void Game::DestroyGameObject(GameObject* to_destroy) {
        Networking::DespawnPacket* despawn = new Networking::DespawnPacket();
        despawn->unit = to_destroy->unit_id;
        SendPacket(despawn);
        to_destroy->is_destroyed = true;
    }

    void Game::CheckCollision(GameObject* collider) {
        for (auto go_it : game_objects_) {
            GameObject* go = go_it.second;

            if (go->unit_id == collider->unit_id) {
                continue;
            }

            if (Physics::TestCollision(
                Physics::Circle({ go->position.x, go->position.z }, go->collision_radius),
                Physics::Circle({ collider->position.x, collider->position.z }, collider->collision_radius))
                ) {
                collider->OnCollision(this, go);
            }
        }
    }

    void Game::Update(float dt) {
        lastTick += dt;

        // ca. 16 ms per tick
        if (lastTick < TICKRATE / 1000.0f) {
            return;
        }

        // Next gametick -> wrap to zero at start. Yikes...
        lastTick -= TICKRATE / 1000.0f;

        // collide
        for (auto go_it : game_objects_) {
            GameObject* go = go_it.second;
            CheckCollision(go);
        }

        // make sure we figure out all the buffs and such first
        // also passive regen and whatnot
        for (auto go_it : game_objects_) {
            GameObject* go = go_it.second;
            go->Update(dt, this);
        }

        for (auto go_it : igame_objects_) {
            IGameObject* go = go_it.second;
            go->Update(this, dt);
        }

        // now let them cook?
        for (auto go_it : game_objects_) {
            GameObject* go = go_it.second;
            go->controller->Think(this, go);
        }

        // ok, action
        for (auto go_it : game_objects_) {
            GameObject* go = go_it.second;
            go->Think(dt, this);
        }

        packet_t packet{};
        packet.header.type = PacketType::GAME_TICK;

        for (packet_t pack : tick_packets) {
            packet << pack;
            if (pack.header.size == 0) {
                throw std::exception();
            }
        }

        for (Networking::BasePacket* base : tick_base_packets_) {
            std::vector<uint8_t> appendage;
            base->Write(&appendage);
            
            packet.data.resize(packet.data.size() + appendage.size());

            // TODO pls fixerino
            if (packet.header.size == 0) {
                std::memcpy(packet.data.data(), appendage.data(), appendage.size());
            }
            else {
                std::memcpy(packet.data.data() + packet.header.size - sizeof(packet_header_t), appendage.data(), appendage.size());
            }
            packet.header.size = packet.size();
        }

        packet << gameTick++;
        all_ticks.push_back(packet);

        on_sendToAllClients(&packet);

        tick_packets.clear();
        tick_base_packets_.clear();

        // TODO why is this stupid
        std::erase_if(game_objects_, [](auto& kv) {
            if (kv.second->is_destroyed) {
                delete kv.second;
                return true;
            }

            return false;
        });
    }
}
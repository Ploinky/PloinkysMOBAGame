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
#include "person.h"

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

        packet_manager = new Networking::NetworkHandlerManager<Networking::PacketType>();

        entity_id id = current_entity_id_++;


        Networking::UnitIdPacket packet = Networking::UnitIdPacket();
        packet.unit_id = id;
        std::vector<uint8_t> data;
        packet.Write(&data);
        on_sendToClient(netId, &data);

        FootballPerson* game_object = new FootballPerson();
        game_object->current_action_ = nullptr;
        game_object->unit_id = id;

        if (players_.size() % 2 == 0) {
            game_object->team = Team::TEAM_1;
        }
        else {
            game_object->team = Team::TEAM_2;
        }

        igame_objects_.emplace(id, game_object);

        player_t player{};
        player.networkId = netId;
        player.unitId = id;
        players_.emplace(netId, player);

        Networking::SpawnPacket* spawn = new Networking::SpawnPacket();
        spawn->unit = game_object->unit_id;
        spawn->unit_type = UnitPrefab::FOOTBALL_PERSON;
        spawn->team = game_object->team;
        spawn->x = 0;
        spawn->y = 0;
        SendPacket(spawn);

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

        IGameObject* go = igame_objects_.find(players_.find(netId)->second.unitId)->second;
        igame_objects_.erase(go->unit_id);
        delete go;
    }

    void Game::AddGameObject(IGameObject* game_object) {
        game_object->unit_id = current_entity_id_++;
        igame_objects_.emplace(game_object->unit_id, game_object);
    }

    void Game::SpawnMissile(Missile* missile) {
        // adjust target point here???
        Physics::Vector3 dir = missile->target_point - missile->position;
        dir = dir.Normalize();

        dir = dir * missile->max_distance;

        missile->target_point = missile->position + dir;

        missile->unit_id = current_entity_id_++;
        igame_objects_.emplace(missile->unit_id, missile);


        Networking::SpawnPacket* spawn = new Networking::SpawnPacket();
        spawn->unit = missile->unit_id;
        spawn->unit_type = UnitPrefab::THROW_FOOTBALL;
        spawn->team = missile->team;
        spawn->x = missile->position.x;
        spawn->y = missile->position.y;
        SendPacket(spawn);
    }

    void Game::PlayerMoveCommand(unsigned long netId, float nx, float ny) {
        ((Person*)igame_objects_.find(players_.find(netId)->second.unitId)->second)->current_action_ = new GameObjectActionMove({ nx, ny, 0 });
    }

    void Game::PlayerStopCommand(unsigned long netId) {

        ((Person*)igame_objects_.find(players_.find(netId)->second.unitId)->second)->current_action_ = new GameObjectActionStop();
    }

    void Game::PlayerAttackCommand(unsigned long netId, unsigned long target_id) {
        IGameObject* actor = igame_objects_.find(players_.find(netId)->second.unitId)->second;
        IGameObject* target = GetGameObjectById(target_id);
        ((Attackable*)actor)->current_action_ = new GameObjectActionAttackUnit(target_id);
    }

    void Game::PlayerCastSpellCommand(unsigned long netId, int spell_slot, SpellTargetInfo* target_info) {
        IGameObject* actor = GetGameObjectById(players_.find(netId)->second.unitId);

        if (((Person*)actor)->spells[spell_slot]->remaining_cooldown != -1) {
            // nope!
            return;
        }

        GameObjectActionCastSpell* new_action = new GameObjectActionCastSpell(spell_slot);
        new_action->target_info = target_info;
        ((Person*)actor)->current_action_ = new_action;
    }

    void Game::Start() {
        Building* tower = new Building(Team::TEAM_2);
        tower->position = { 10, 0, 0 };
        AddGameObject(tower);

        Building* tower2 = new Building(Team::TEAM_1);
        tower2->position = { -10, 0, 0 };
        AddGameObject(tower2);

        MinionSpawner* minion_spawner = new MinionSpawner();
        igame_objects_.emplace(0, minion_spawner);
    }

    void Game::DestroyGameObject(IGameObject* to_destroy) {
        Networking::DespawnPacket* despawn = new Networking::DespawnPacket();
        despawn->unit = to_destroy->unit_id;
        SendPacket(despawn);
        to_destroy->is_destroyed = true;
    }

    void Game::CheckCollision(IGameObject* collider) {
        if (Attackable* coll = dynamic_cast<Attackable*>(collider)) {
            for (auto go_it : igame_objects_) {
                IGameObject* go = go_it.second;

                if (go->unit_id == collider->unit_id) {
                    continue;
                }

                if (Attackable* oth = dynamic_cast<Attackable*>(go)) {
                    if (Physics::TestCollision(
                        Physics::Circle({ oth->position.x, oth->position.z }, go->collision_radius),
                        Physics::Circle({ coll->position.x, coll->position.z }, collider->collision_radius))
                        ) {
                        collider->OnCollision(this, go);
                    }
                }
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

        // make sure we figure out all the buffs and such first
        // also passive regen and whatnot
        for (auto go_it : igame_objects_) {
            IGameObject* go = go_it.second;
            go->Update(this, dt);
        }

        // ok, action
        for (auto go_it : igame_objects_) {
            IGameObject* go = go_it.second;
            go->Act(this, dt);
        }

        Networking::packet_header_t header{};
        header.type = Networking::PacketType::GAME_TICK;

        std::vector<uint8_t> data;
        data.resize(sizeof(header));

        header.size = data.size();

        for (Networking::BasePacket* base : tick_packets_) {
            std::vector<uint8_t> appendage;
            base->Write(&appendage);
            
            data.resize(data.size() + appendage.size());

            // TODO pls fixerino
            if (header.size == 0) {
                std::memcpy(data.data(), appendage.data(), appendage.size());
            }
            else {
                std::memcpy(data.data() + header.size, appendage.data(), appendage.size());
            }
            header.size = data.size();
        }

        for (auto go_it : igame_objects_) {
            IGameObject* go = go_it.second;

            std::vector<uint8_t> pck_data;
            go->Sync(&pck_data);
            
            if (pck_data.size() == 0) {
                // nothing to sync?
                continue;
            }
            data.resize(data.size() + pck_data.size());

            if (header.size == 0) {
                std::memcpy(data.data(), pck_data.data(), pck_data.size());
            }
            else {
                std::memcpy(data.data() + header.size, pck_data.data(), pck_data.size());
            }
            header.size = data.size();
        }

        data.resize(data.size() + sizeof(gameTick));
        std::memcpy(data.data(), &gameTick, sizeof(gameTick));
        gameTick++;
        header.size += sizeof(gameTick);
        std::memcpy(data.data(), &header, sizeof(header));

        all_ticks.push_back(data);

        on_sendToAllClients(&data);

        tick_packets_.clear();

        // TODO why is this stupid
        std::erase_if(igame_objects_, [](auto& kv) {
            if (kv.second->is_destroyed) {
                delete kv.second;
                return true;
            }

            return false;
        });
    }
}
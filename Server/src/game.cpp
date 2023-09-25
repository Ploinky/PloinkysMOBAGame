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
    }

    void Game::RemovePlayerForNetworkId(unsigned long netId) {
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

        AddGameObject(missile);
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
        Building* tower = new Building(Team::TEAM_1);
        tower->position = { -20, 0, 2 };
        AddGameObject(tower);

        Building* tower2 = new Building(Team::TEAM_1);
        tower2->position = { -10, 0, 2 };
        AddGameObject(tower2);

        Building* tower3 = new Building(Team::TEAM_2);
        tower3->position = { 10, 0, 2 };
        AddGameObject(tower3);

        Building* tower4 = new Building(Team::TEAM_2);
        tower4->position = { 20, 0, 2 };
        AddGameObject(tower4);

        MinionSpawner* minion_spawner = new MinionSpawner({ {30, 0, 0} });
        minion_spawner->position = { -25, 0, 0 };
        minion_spawner->team = Team::TEAM_1;
        AddGameObject(minion_spawner);

        MinionSpawner* minion_spawner2 = new MinionSpawner({ {-30, 0, 0} });
        minion_spawner2->position = { 25, 0, 0 };
        minion_spawner2->team = Team::TEAM_2;
        AddGameObject(minion_spawner2);
    }

    void Game::CheckCollision(IGameObject* collider) {
        for (auto go_it : igame_objects_) {
            IGameObject* go = go_it.second;

            if (go->unit_id == collider->unit_id) {
                continue;
            }

            if (Physics::TestCollision(
                Physics::Circle({ collider->position.x, collider->position.z }, collider->collision_radius),
                Physics::Circle({ go->position.x, go->position.z }, 0))
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

        // make sure we figure out all the buffs and such first
        // also passive regen and whatnot
        for (auto go_it : igame_objects_) {
            IGameObject* go = go_it.second;
            go->Update(this, dt);
        }

        for (auto go_it : igame_objects_) {
            IGameObject* go = go_it.second;
            CheckCollision(go);
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

        for (auto go_it : igame_objects_) {
            IGameObject* go = go_it.second;
            go->Sync(&data);
        }

        header.size = data.size();

        data.resize(data.size() + sizeof(gameTick));
        std::memcpy(data.data() + header.size, &gameTick, sizeof(gameTick));
        gameTick++;
        header.size += sizeof(gameTick);
        std::memcpy(data.data(), &header, sizeof(header));

        all_ticks.push_back(data);

        on_sendToAllClients(&data);

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
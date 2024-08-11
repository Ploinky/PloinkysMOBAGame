#include "Game.h"
#include "Components.h"
#include "Common/pmg_physics.h"
#include "GameObject.h"
#include "Missile.h"
#include "Building.h"
#include "FootballPerson.h"
#include "MinionSpawner.h"
#include "Common/pmg_networking.h"
#include "Person.h"
#include "Minion.h"
#include <cstring>
#include "IServerState.h"
#include "IServerStateHandler.h"

namespace PMG {
    uint64_t g_unitId = 0;

    Client::Client(IServerStateHandler* handler, ServerNetworkManager* networkManager, AssetManager* assetManager, LobbyPlayer* players[10]) : IServerState(handler) {
        for (int i = 0; i < 10; i++) {
            if (players[i] != nullptr) {
                players_.emplace(players[i]->steamId, players[i]);
            }
        }
        
        m_navMesh = new NavMesh();
        m_navMesh->LoadFromData(assetManager->LoadPlainFile("Maps/map1\\map1.nvm"));

		m_navGrid = new NavigationCellGrid(m_navMesh);

        networkManager_ = networkManager;
        networkManager_->on_clientMessageReceived = [this](PlayerID playerId, std::vector<uint8_t>* data) {
            OnMessageReceived(playerId, data);
        };

        networkManager_->on_clientConnected = [this](PlayerID newPlayerId) {
            std::map<PlayerID, LobbyPlayer*>::iterator playerIt = players_.find(newPlayerId);

            if (playerIt == players_.end()) {
                // TODO reject players that are not part of the game!
                printf("Player is not in game!\n");
                return;
            }

            LobbyPlayer* player = playerIt->second;

			if(player->ready) {
				// TODO wtf the player is still connected?
				printf("Player already connected?!\n");
				return;
			}

            Networking::LobbyGameStartPck pck;

            std::vector<uint8_t> data;
            pck.Write(&data);

            SendMessageToClient(player->steamId, &data);
            data.clear();
            data.resize(0);

            Networking::UnitIdPacket packet = Networking::UnitIdPacket();
            packet.unit_id = player->unit;
            packet.Write(&data);

            SendMessageToClient(player->steamId, &data);

            for (auto tick : all_ticks) {
                SendMessageToClient(player->steamId, &tick);
            }
        };

        networkManager_->on_clientDisconnected = [this](PlayerID playerId) {
            // TODO something should probably happen

            std::map<PlayerID, LobbyPlayer*>::iterator playerIt = players_.find(playerId);

            if (playerIt == players_.end()) {
                // TODO reject players that are not part of the game!
                printf("Player is not in game!\n");
                return;
            }

            LobbyPlayer* player = playerIt->second;
			player->ready = false;

			for(auto player : players_) {
				if(player.second != nullptr && player.second->ready) {
					// TODO
					// for now we just get out
					return;
				}
			}
			
			handler_->StartLobby(networkManager_);
        };
    }

    void Client::AddPlayerForNetworkId(int index, LobbyPlayer* player) {
        for (auto tick : all_ticks) {
            SendMessageToClient(player->steamId, &tick);
        }

        packet_manager = new Networking::NetworkHandlerManager<Networking::PacketType, std::function<void(std::vector<uint8_t>)>>();

        entity_id id = current_entity_id_++;

        Networking::UnitIdPacket packet = Networking::UnitIdPacket();
        packet.unit_id = id;
        std::vector<uint8_t> data;
        packet.Write(&data);
        player->unit = id;
        SendMessageToClient(player->steamId, &data);

        FootballPerson* game_object = new FootballPerson();
        game_object->DoAction(nullptr);
        game_object->unit_id = id;
        game_object->nav_agent.grid = m_navGrid;

        if (index % 2 == 0) {
            game_object->team = Team::TEAM_1;
			game_object->position = { 1000, 0 , -2500};
        }
        else {
            game_object->team = Team::TEAM_2;
			game_object->position = { 4000, 0 , -2500};
        }

        igame_objects_.emplace(id, game_object);
    }

    void Client::AddGameObject(IGameObject* game_object) {
        game_object->unit_id = current_entity_id_++;
        igame_objects_.emplace(game_object->unit_id, game_object);
    }

    void Client::SpawnMissile(Missile* missile) {
        // adjust target point here???
        Physics::Vector3 dir = missile->target_point - missile->position;
        dir = dir.Normalize();

        dir = dir * missile->max_distance;

        missile->target_point = missile->position + dir;

        AddGameObject(missile);
    }

    void Client::PlayerMoveCommand(PlayerID playerId, float nx, float ny) {
        if (players_.find(playerId) != players_.end()) {
            LobbyPlayer* player = players_.find(playerId)->second;
            Person* person = ((Person*)igame_objects_.find(player->unit)->second);
            person->DoAction(new GameObjectActionMove(person, { nx, ny, 0 }, this));
        }
    }

    void Client::PlayerStopCommand(PlayerID playerId) {
        if (players_.find(playerId) != players_.end()) {
            LobbyPlayer* player = players_.find(playerId)->second;
            Person* person = (Person*)igame_objects_.find(player->unit)->second;
            person->DoAction(new GameObjectActionStop(person, this));
        }
    }

    void Client::PlayerAttackCommand(PlayerID playerId, uint64_t target_id) {
        if (players_.find(playerId) != players_.end()) {
            IGameObject* actor = igame_objects_.find(players_.find(playerId)->second->unit)->second;
            IGameObject* target = GetGameObjectById(target_id);
            ((Attackable*)actor)->DoAction(new GameObjectActionAttackUnit(actor->unit_id, target->unit_id, this));
        }
    }

    void Client::PlayerCastSpellCommand(PlayerID playerId, int spell_slot, SpellTargetInfo* target_info) {
        if (players_.find(playerId) != players_.end()) {
            IGameObject* actor = GetGameObjectById(players_.find(playerId)->second->unit);

            if (((Person*)actor)->spells[spell_slot]->remaining_cooldown != -1) {
                // nope!
                return;
            }

            GameObjectActionCastSpell* new_action = new GameObjectActionCastSpell((Person*) actor, spell_slot, this);
            new_action->target_info = target_info;
            ((Person*)actor)->DoAction(new_action);
        }
    }

    void Client::Start() {
        Building* tower = new Building(Team::TEAM_1);
        tower->position = { 2550, 0, -2750 };
        AddGameObject(tower);

        Building* tower2 = new Building(Team::TEAM_1);
        tower2->position = { 3550, 0, -2750 };
        AddGameObject(tower2);

        Building* tower3 = new Building(Team::TEAM_2);
        tower3->position = { 5550, 0, -2750 };
        AddGameObject(tower3);

        Building* tower4 = new Building(Team::TEAM_2);
        tower4->position = { 6550, 0, -2750 };
        AddGameObject(tower4);

        MinionSpawner* minion_spawner = new MinionSpawner({ {7500, 0, -2500} });
        minion_spawner->position = { 2000, 0, -2500 };
        minion_spawner->team = Team::TEAM_1;
        AddGameObject(minion_spawner);

        MinionSpawner* minion_spawner2 = new MinionSpawner({ {1500, 0, -2500} });
        minion_spawner2->position = { 7000, 0, -2500 };
        minion_spawner2->team = Team::TEAM_2;
        AddGameObject(minion_spawner2);

        for (auto playerIt : players_) {
            // TODO slot as well!
            AddPlayerForNetworkId(playerIt.second->slot, playerIt.second);
        }
    }

    void Client::CheckCollision(IGameObject* collider) {
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

    void Client::Update(float dt) {
        lastTick += dt;

        // ca. 16 ms per tick
        if (lastTick < TICKRATE / 1000.0f) {
            return;
        }

        // Next gametick -> wrap to zero at start. Yikes...
        lastTick -= TICKRATE / 1000.0f;

        // first lets see what the clients have to say
        networkManager_->Update();

		for(auto index = 0; index < m_navGrid->CellCountX * m_navGrid->CellCountY; index++) {
			m_navGrid->Cells[index]->IsOpen = true;
		}

		for(auto go_it : igame_objects_) {
            IGameObject* go = go_it.second;

			if(Attackable* attackable = dynamic_cast<Attackable*>(go)) {
				m_navGrid->GetCellAt(attackable->position.x, attackable->position.z)->IsOpen = false;
			}
		}

        // make sure we figure out all the buffs and such first
        // also passive regen and whatnot
        for (auto go_it : igame_objects_) {
            IGameObject* go = go_it.second;
            go->Update(this, TICKRATE / 1000.0f);
        }

        // ok, action
        for (auto go_it : igame_objects_) {
            IGameObject* go = go_it.second;
            go->Act(this, TICKRATE / 1000.0f);
        }

        for (auto go_it : igame_objects_) {
            IGameObject* go = go_it.second;
            CheckCollision(go);
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
        memcpy(data.data() + header.size, &gameTick, sizeof(gameTick));
        gameTick++;
        header.size += sizeof(gameTick);
        memcpy(data.data(), &header, sizeof(header));

        all_ticks.push_back(data);

        SendMessageToAllClients(&data);

        // TODO why is this stupid
        erase_if(igame_objects_, [](auto& kv) {
            if (kv.second->is_destroyed) {
                delete kv.second;
                return true;
            }

            return false;
            });
    }

    void Client::OnMessageReceived(PlayerID playerId, std::vector<uint8_t>* data) {
        Networking::packet_header_t header{};
        memcpy(&header, data->data(), sizeof(header));

        switch (header.type) {
        case Networking::PacketType::UNITMOVE: {
            Networking::MoveCommandPacket move_command = Networking::MoveCommandPacket();
            move_command.Read(data);

            PlayerMoveCommand(playerId, move_command.x, move_command.y);
            break;
        }
        case Networking::PacketType::CMD_STOP: {
            Networking::StopCommandPacket move_command = Networking::StopCommandPacket();
            move_command.Read(data);

            PlayerStopCommand(playerId);
            break;
        }
        case Networking::PacketType::CMD_ATTACK: {
            Networking::AttackCommandPacket atk_command = Networking::AttackCommandPacket();
            atk_command.Read(data);

            PlayerAttackCommand(playerId, atk_command.target_unit);
            break;
        }
        case Networking::PacketType::CMD_CAST: {
            Networking::CastCommandPacket cast = Networking::CastCommandPacket();
            cast.Read(data);


            SpellTargetInfo* target_info = new SpellTargetInfo();
            target_info->target_point = { cast.x, cast.y, cast.z };
            PlayerCastSpellCommand(playerId, cast.spell_slot, target_info);
            break;
        }
        case Networking::PacketType::CMD_CAST_TARGET: {
            Networking::CastTargetCommandPacket cast_command = Networking::CastTargetCommandPacket();
            cast_command.Read(data);

            SpellTargetInfo* target_info = new SpellTargetInfo();
            target_info->target = dynamic_cast<Attackable*>(GetGameObjectById(cast_command.target));
            PlayerCastSpellCommand(playerId, cast_command.spell_slot, target_info);
            break;
        }
        }
    }
}
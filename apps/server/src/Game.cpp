#include "Game.h"
#include "Components.h"
#include "common/pmg_physics.h"
#include "Missile.h"
#include "common/pmg_networking.h"
#include "GameObject.h"
#include <cstring>
#include "IServerState.h"
#include "IServerStateHandler.h"
#include "systems/NavigationSystem.h"
#include "Components.h"
#include "SpellTargetInfo.h"
#include "events/spell-cast-start-event.h"
#include "events/damage-event.h"
#include "events.h"
#include "systems/respawn-system.h"
#include "systems/attack-system.h"
#include "systems/ai-system.h"
#include "systems/cleanup-system.h"
#include "systems/inventory-system.h"
#include "systems/spawn-system.h"
#include "systems/trigger-system.h"
#include <common/data/game-data.h>

uint64_t g_unitId = 0;

Client::Client(IServerStateHandler* handler, ServerNetworkManager* networkManager, AssetManager* assetManager, LobbyPlayer* players[10]) : IServerState(handler) {
    for (int i = 0; i < 10; i++) {
        if (players[i] != nullptr) {
            players_.emplace(players[i]->idPlayer, players[i]);
        }
    }
    
#ifndef _DEBUG
    assetManager->LoadPakFile("assets/maps/map1.pak");
#endif

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

        LobbyGameStartPck pck;

        std::vector<uint8_t> data;
        pck.Write(&data);

        SendMessageToClient(player->idPlayer, &data);
        data.clear();
        data.resize(0);

        UnitIdPacket packet = UnitIdPacket();
        packet.unit_id = player->unit;
        packet.Write(&data);

        SendMessageToClient(player->idPlayer, &data);

        for (auto tick : all_ticks) {
            SendMessageToClient(player->idPlayer, &tick);
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
        
        LobbyPlayer* players[10] = {0};
        handler_->ResumeLobby(networkManager_, players);
    };

    m_pNetworkSystem = new CNetworkSystem(networkManager_);
    NavMesh* pNavMesh = new NavMesh();
    pNavMesh->LoadFromData(assetManager->LoadPlainFile(handler_->GetGameData().mapMapData.at("map1").navMeshDataFile));
    m_game.Start(handler_->GetGameData(), pNavMesh);
    m_game.GetState()->AddSystem(m_pNetworkSystem);
}

void Client::AddPlayerForNetworkId(int index, LobbyPlayer* player) {
    for (auto tick : all_ticks) {
        SendMessageToClient(player->idPlayer, &tick);
    }

    UnitId id = m_game.AddPlayer();
    if (index == 0 || index == 2 || index == 4 || index == 6 || index == 8) {
        m_game.GetState()->AddTeam(id, TeamComponent_t(Team::TEAM_1));
    } else {
        m_game.GetState()->AddTeam(id, TeamComponent_t(Team::TEAM_2));
    }

    // inform everybody TODO prettify
    UnitIdPacket packet = UnitIdPacket();
    packet.unit_id = id;
    std::vector<uint8_t> data;
    packet.Write(&data);
    player->unit = id;
    SendMessageToClient(player->idPlayer, &data);
}

void Client::PlayerMoveCommand(PlayerID playerId, float nx, float ny) {
    if (players_.find(playerId) != players_.end()) {
        LobbyPlayer* player = players_.find(playerId)->second;

        m_game.Input(CMoveAttemptEvent(
            player->unit,
            {nx, 0, ny},
            0
        ));
    }
}

void Client::PlayerStopCommand(PlayerID playerId) {
    if (players_.find(playerId) != players_.end()) {
        LobbyPlayer* player = players_.find(playerId)->second;

        m_game.Input(CStopEvent(player->unit));
        m_game.Input(CEntityIdleEvent(player->unit));
    }
}

void Client::PlayerAttackCommand(PlayerID playerId, uint64_t target_id) {
    if (players_.find(playerId) != players_.end()) {
        m_game.Input(CAttackIntentionEvent(players_.find(playerId)->second->unit, target_id));
    }
}

void Client::PlayerCastSpellCommand(PlayerID playerId, int spell_slot, SpellTargetInfo* target_info) {
    if (players_.find(playerId) == players_.end()) {
        Logger::FormatErr("Invalid cast command: unknown playerId %d", playerId);
        return;
    }

    m_game.Input(CSpellAttemptCastEvent(players_.find(playerId)->second->unit, *target_info, spell_slot));
}

void Client::Start() {
    for (auto playerIt : players_) {
        // TODO slot as well!
        AddPlayerForNetworkId(playerIt.second->slot, playerIt.second);
    }

    UnitId dummy = m_game.GetState()->SpawnUnit(*m_game.GetState()->m_pGameData, "stormcaller", Vector2(1000, -1000));
    m_game.GetState()->AddTeam(dummy)->eTeam = Team::TEAM_2;
    m_game.GetState()->AddRespawn(dummy);

    UnitId dummy2 = m_game.GetState()->SpawnUnit(*m_game.GetState()->m_pGameData, "stormcaller", Vector2(1000, -800));
    m_game.GetState()->AddTeam(dummy2)->eTeam = Team::TEAM_2;
    m_game.GetState()->AddRespawn(dummy2);

    m_game.GetState()->SpawnUnit(*m_game.GetState()->m_pGameData, "device", Vector2(750, -750));
}

void Client::Update(float dt) {
    // first lets see what the clients have to say
    networkManager_->Update();

    // TODO unify
    if (m_game.GetState()->bIsGameEnded) {
        GameEndPacket pck;
        pck.victory = true;
        networkManager_->SendToAllClientsReliable(pck);
        LobbyPlayer* players[10] = {0};
        int c = 0;
        for (auto it = players_.begin(); c < 10 && it != players_.end(); it++) {
            players[c++] = it->second;
        }
        handler_->ResumeLobby(networkManager_, players);
        return;
    }

    m_game.Tick();

    m_pNetworkSystem->SyncGameState(m_game.GetState());
}

void Client::OnMessageReceived(PlayerID playerId, std::vector<uint8_t>* data) {
    packet_header_t header{};
    memcpy(&header, data->data(), sizeof(header));

    switch (header.type) {
    case PacketType::MOVE_CMD: {
        MoveCommandPacket move_command = MoveCommandPacket();
        move_command.Read(data);

        PlayerMoveCommand(playerId, move_command.x, move_command.y);
        break;
    }
    case PacketType::CMD_STOP: {
        StopCommandPacket move_command = StopCommandPacket();
        move_command.Read(data);

        PlayerStopCommand(playerId);
        break;
    }
    case PacketType::CMD_ATTACK: {
        AttackCommandPacket atk_command = AttackCommandPacket();
        atk_command.Read(data);

        PlayerAttackCommand(playerId, atk_command.target_unit);
        break;
    }
    case PacketType::CMD_CAST: {
        CastCommandPacket cast = CastCommandPacket();
        cast.Read(data);


        SpellTargetInfo* target_info = new SpellTargetInfo();
        target_info->target_point = { cast.x, cast.y, cast.z };
        PlayerCastSpellCommand(playerId, cast.spell_slot, target_info);
        break;
    }
    case PacketType::CMD_CAST_TARGET: {
        CastTargetCommandPacket cast_command = CastTargetCommandPacket();
        cast_command.Read(data);

        SpellTargetInfo* target_info = new SpellTargetInfo();
        target_info->target = cast_command.target;
        PlayerCastSpellCommand(playerId, cast_command.spell_slot, target_info);
        break;
    }
    case PacketType::CMD_PICK_UP_ENTITY: {
        CPickUpEntityCommand pickUpCommand = CPickUpEntityCommand();
        pickUpCommand.Read(data);

        LobbyPlayer* pPlayer = players_.at(playerId);
        m_game.Input(CPickUpAttemptEvent(pPlayer->unit, pickUpCommand.idUnit));
        break;
    }
    case PacketType::CMD_USE_ENTITY_POINT: {
        CUseEntityPointCommand useCommand = CUseEntityPointCommand();
        useCommand.Read(data);

        LobbyPlayer* pPlayer = players_.at(playerId);
        CUseEntityAttemptEvent evt = CUseEntityAttemptEvent();
        evt.idUser = pPlayer->unit;
        evt.idEntity = useCommand.idEntity;
        m_game.Input(evt);
    }
    }
}
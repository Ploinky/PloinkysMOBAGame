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
#include "systems/points-system.h"
#include "systems/respawn-system.h"
#include "systems/attack-system.h"
#include "systems/ai-system.h"
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

    m_navMesh = new NavMesh();
    m_navMesh->LoadFromData(assetManager->LoadPlainFile(handler_->GetGameData().mapMapData.at("map1").navMeshDataFile));

    m_navMap = new NavigationMap();
    m_navMap->m_pMesh = m_navMesh;
    m_navMap->m_pGrid = new NavigationCellGrid(m_navMesh);


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
        
        handler_->StartLobby(networkManager_);
    };

    m_pNetworkSystem = new CNetworkSystem(networkManager_);
    m_pNavigationSystem = new CNavigationSystem(m_navMap);
    GameState.SetNavMap(m_navMap);
    GameState.m_pGameData = &handler_->GetGameData();

    // =====================================
    // THIS NEEDS TO GO SOMEWHERE
    // THIS IS A MESS AND MAKES NO SENSE
    // THIS IS GETTING WORSE AND WORSE
    // HOW WILL I EVER MAKE A GAME WHEN I WRITE CODE LIKE THIS
    // WHY AM I TOO STUPID TO MAKE IT WORK
    // IT'S NOT ALL THAT DIFFICULT
    // PLEASE FIGURE IT TF OUT
    
    CTriggerData trigData = handler_->GetGameData().mapTriggerData.at("wave_team_1");
    CTrigger trigger = CTrigger();
    for(CTriggerSpawnUnitData spawnTriggerData : trigData.vecSpawnUnitTriggers) {
        CSpawnUnitTrigger spawnUnitTrigger;
        spawnUnitTrigger.eTeam = spawnTriggerData.eTeam;
        spawnUnitTrigger.idUnitType = spawnTriggerData.idUnitType;
        spawnUnitTrigger.nCount = spawnTriggerData.nCount;
        spawnUnitTrigger.nTime = spawnTriggerData.nTime;
        spawnUnitTrigger.nLastTime = -1;
        spawnUnitTrigger.vec2Position = spawnTriggerData.vec2Position;
        trigger.vecSpawnUnitTriggers.push_back(spawnUnitTrigger);
    }
    GameState.VecTriggers.push_back(trigger);
    // =====================================

    GameState.AddSystem(m_pNavigationSystem);
    GameState.AddSystem(&m_moveSystem);
    GameState.AddSystem(&m_spellSystem);
    GameState.AddSystem(&m_damageSystem);
    GameState.AddSystem(m_pNetworkSystem);
    GameState.AddSystem(new CPointsSystem());
    GameState.AddSystem(new CRespawnSystem());
    GameState.AddSystem(new CAttackSystem());
    GameState.AddSystem(new CAiSystem());
    GameState.AddSystem(new CInventorySystem());
    GameState.AddSystem(new CSpawnSystem(&handler_->GetGameData()));
    GameState.AddSystem(new CTriggerSystem());
}

void Client::AddPlayerForNetworkId(int index, LobbyPlayer* player) {
    for (auto tick : all_ticks) {
        SendMessageToClient(player->idPlayer, &tick);
    }

    // always present
    UnitId id = GameState.SpawnUnit(handler_->GetGameData(), "stormcaller");

    GameState.AddTeam(id, TeamComponent_t(Team::TEAM_1));
    GameState.AddBasicAttack(id, BasicAttackComponent_t());
    GameState.GetTransform(id)->SetPosition({250, 0, -750}); // TODO in data
    GameState.GetMovement(id)->vec3Target = GameState.GetTransform(id)->GetPosition(); // TODO how to make this one call
    GameState.GetNavigation(id)->vec3Destination = GameState.GetTransform(id)->GetPosition();

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

        GameState.EmitEvent(new CMoveAttemptEvent(
            player->unit,
            {nx, 0, ny},
            0
        ));
    }
}

void Client::PlayerStopCommand(PlayerID playerId) {
    if (players_.find(playerId) != players_.end()) {
        LobbyPlayer* player = players_.find(playerId)->second;

        if(MovementComponent_t* pMovement = GameState.GetMovement(player->unit)) {
            if(TransformComponent_t* pTransform = GameState.GetTransform(player->unit)) {
                pMovement->vec3Target = pTransform->GetPosition();
            }
        }
        if(NavigationComponent_t* pNavComp = GameState.GetNavigation(player->unit)) {
            // TODO this belongsn't here!
            pNavComp->eStatus = ENavigationStatus::IDLE;
            pNavComp->vec3Destination = Vector3::ZERO;
            
            Vector3 pos = GameState.GetTransform(player->unit)->GetPosition();
            GameState.EmitEvent(new CMoveIntentionEvent(player->unit, pos, 0));
        }
    }
}

void Client::PlayerAttackCommand(PlayerID playerId, uint64_t target_id) {
    if (players_.find(playerId) != players_.end()) {
        GameState.EmitEvent(new CAttackIntentionEvent(players_.find(playerId)->second->unit, target_id));
    }
}

void Client::PlayerCastSpellCommand(PlayerID playerId, int spell_slot, SpellTargetInfo* target_info) {
    if (players_.find(playerId) == players_.end()) {
        Logger::FormatErr("Invalid cast command: unknown playerId %d", playerId);
        return;
    }

    CSpellAttemptCastEvent* pSpellEvent = new CSpellAttemptCastEvent(players_.find(playerId)->second->unit, *target_info, spell_slot);
    GameState.EmitEvent(pSpellEvent);
}

void Client::Start() {
    for (auto playerIt : players_) {
        // TODO slot as well!
        AddPlayerForNetworkId(playerIt.second->slot, playerIt.second);
    }

    UnitId id = GameState.SpawnUnit(handler_->GetGameData(), "dummy");
    GameState.GetTransform(id)->SetPosition({1000, 0, -1000});
    GameState.GetMovement(id)->vec3Target = {1000, 0, -1000};
    GameState.GetHealth(id)->nHealth = 10;
    GameState.AddTeam(id, TeamComponent_t(Team::TEAM_2));

    UnitId spawnerId = GameState.SpawnUnit(handler_->GetGameData(), "minion_spawner");
    GameState.AddTeam(spawnerId, TeamComponent_t(Team::TEAM_1));
    GameState.GetTransform(spawnerId)->SetPosition({500, 0, -750});
}

void Client::Update(float dt) {
    // first lets see what the clients have to say
    networkManager_->Update();

    for(IGameSystem<CServerGameState>* system : GameState.AllSystems()) {
        system->Update(&GameState, TICKRATE);
    }
    
    for(IGameSystem<CServerGameState>* system : GameState.AllSystems()) {
        system->Finalize(&GameState);
    }

    m_pNetworkSystem->SyncGameState(&GameState);
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

        PickupableComponent_t* pPickupable = GameState.GetPickupable(pickUpCommand.idUnit);

        if(pPickupable == nullptr) {
            Logger::FormatErr("Failed to pick up entity <%d>; entity is not pickupable", pickUpCommand.idUnit);
            break;
        }

        LobbyPlayer* pPlayer = players_.at(playerId);
        CPickUpAttemptEvent* evt = new CPickUpAttemptEvent(pPlayer->unit, pickUpCommand.idUnit);
        GameState.EmitEvent(evt);
        break;
    }
    case PacketType::CMD_USE_ENTITY_POINT: {
        CUseEntityPointCommand useCommand = CUseEntityPointCommand();
        useCommand.Read(data);

        LobbyPlayer* pPlayer = players_.at(playerId);
        CUseEntityAttemptEvent* evt = new CUseEntityAttemptEvent();
        evt->idUser = pPlayer->unit;
        evt->idEntity = useCommand.idEntity;
        GameState.EmitEvent(evt);
    }
    }
}
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

    m_navGrid = new NavigationCellGrid(m_navMesh);
    m_navMap = new NavigationMap();
    m_navMap->m_pGrid = m_navGrid;
    m_navMap->m_pMesh = m_navMesh;


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

    m_vecSystems.push_back(m_pNavigationSystem);
    m_vecSystems.push_back(&m_moveSystem);
    m_vecSystems.push_back(&m_spellSystem);
    m_vecSystems.push_back(&m_damageSystem);
    m_vecSystems.push_back(m_pNetworkSystem);
    m_vecSystems.push_back(new CPointsSystem());
    m_vecSystems.push_back(new CRespawnSystem());
    m_vecSystems.push_back(new CAttackSystem());
    m_vecSystems.push_back(new CAiSystem());
    m_vecSystems.push_back(new CInventorySystem());
}

void Client::AddPlayerForNetworkId(int index, LobbyPlayer* player) {
    for (auto tick : all_ticks) {
        SendMessageToClient(player->idPlayer, &tick);
    }

    // always present
    UnitId id = GameState.SpawnUnit(handler_->GetGameData(), "stormcaller");

    GameState.AddTeam(id, CTeamComponent(Team::TEAM_1));
    GameState.AddBasicAttack(id, CBasicAttackComponent());

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

        GameState.VecEvent.emplace(new CMoveAttemptEvent(
            player->unit,
            {nx, 0, ny},
            0
        ));
    }
}

void Client::PlayerStopCommand(PlayerID playerId) {
    if (players_.find(playerId) != players_.end()) {
        LobbyPlayer* player = players_.find(playerId)->second;

        if(CMovementComponent* pMovement = GameState.GetMovement(player->unit)) {
            if(CTransformComponent* pTransform = GameState.GetTransform(player->unit)) {
                pMovement->vec3Target = pTransform->GetPosition();
            }
        }
        if(CNavigationComponent* pNavComp = GameState.GetNavigation(player->unit)) {
            pNavComp->bIsNavigating = false;
            pNavComp->vec3Destination = Vector3::ZERO;
            
            // TODO this belongsn't here
            Vector3 pos = GameState.GetTransform(player->unit)->GetPosition();
            GameState.VecEvent.emplace(new CMoveIntentionEvent(player->unit, pos, 0));
        }
    }
}

void Client::PlayerAttackCommand(PlayerID playerId, uint64_t target_id) {
    if (players_.find(playerId) != players_.end()) {
        GameState.VecEvent.emplace(new CAttackIntentionEvent(players_.find(playerId)->second->unit, target_id));
    }
}

void Client::PlayerCastSpellCommand(PlayerID playerId, int spell_slot, SpellTargetInfo* target_info) {
    if (players_.find(playerId) == players_.end()) {
        Logger::FormatErr("Invalid cast command: unknown playerId %d", playerId);
        return;
    }

    CSpellAttemptCastEvent* pSpellEvent = new CSpellAttemptCastEvent(players_.find(playerId)->second->unit, *target_info, spell_slot);
    GameState.VecEvent.emplace(pSpellEvent);
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
    GameState.AddTeam(id, CTeamComponent(Team::TEAM_2));

    UnitId idDevice = GameState.SpawnUnit(handler_->GetGameData(), "device");
    GameState.AddTransform(idDevice)->SetPosition({500, 0, -500});
    GameState.AddDevice(idDevice);
}

void Client::Update(float dt) {
    // first lets see what the clients have to say
    networkManager_->Update();

    for(auto index = 0; index < m_navGrid->CellCountX * m_navGrid->CellCountY; index++) {
        m_navGrid->Cells[index]->IsOpen = true;
    }

    for(CTransformComponent& transform : GameState.GetAllTransform()) {
        m_navGrid->GetCellAt(transform.GetPosition().x - 25, transform.GetPosition().z - 25)->IsOpen = false;
        m_navGrid->GetCellAt(transform.GetPosition().x - 25, transform.GetPosition().z - 25)->UnitId = transform.idUnit;
        m_navGrid->GetCellAt(transform.GetPosition().x + 25, transform.GetPosition().z - 25)->IsOpen = false;
        m_navGrid->GetCellAt(transform.GetPosition().x + 25, transform.GetPosition().z - 25)->UnitId = transform.idUnit;
        m_navGrid->GetCellAt(transform.GetPosition().x + 25, transform.GetPosition().z + 25)->IsOpen = false;
        m_navGrid->GetCellAt(transform.GetPosition().x + 25, transform.GetPosition().z + 25)->UnitId = transform.idUnit;
        m_navGrid->GetCellAt(transform.GetPosition().x - 25, transform.GetPosition().z + 25)->IsOpen = false;
        m_navGrid->GetCellAt(transform.GetPosition().x - 25, transform.GetPosition().z + 25)->UnitId = transform.idUnit;
    }
    
    // m_waveManager.Update(&GameState, TICKRATE);
    for(ISystem* system : m_vecSystems) {
        system->Update(&GameState, TICKRATE);
    }
    
    // TODO this appears wrong
    while(!GameState.VecEvent.empty()) {
        IGameEvent* pEvt = GameState.VecEvent.front();
        for(ISystem* system : m_vecSystems) {
            system->Process(&GameState, pEvt);
        }
        GameState.VecEvent.pop();
        delete pEvt;
    }

    for(ISystem* system : m_vecSystems) {
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

        CPickupableComponent* pPickupable = GameState.GetPickupable(pickUpCommand.idUnit);

        if(pPickupable == nullptr) {
            Logger::FormatErr("Failed to pick up entity <%d>; entity is not pickupable", pickUpCommand.idUnit);
            break;
        }

        LobbyPlayer* pPlayer = players_.at(playerId);
        CPickUpAttemptEvent* evt = new CPickUpAttemptEvent(pPlayer->unit, pickUpCommand.idUnit);
        GameState.VecEvent.emplace(evt);
        break;
    }
    }
}
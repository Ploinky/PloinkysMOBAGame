#include "Game.h"
#include "Components.h"
#include "Common/pmg_physics.h"
#include "Missile.h"
#include "Common/pmg_networking.h"
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
#include "content/characters/stormcaller/thunderstrike.h"

uint64_t g_unitId = 0;

Client::Client(IServerStateHandler* handler, ServerNetworkManager* networkManager, AssetManager* assetManager, LobbyPlayer* players[10]) : IServerState(handler) {
    for (int i = 0; i < 10; i++) {
        if (players[i] != nullptr) {
            players_.emplace(players[i]->steamId, players[i]);
        }
    }
    
    assetManager->LoadPakFile("Maps/map1.pak");
    m_navMesh = new NavMesh();
    m_navMesh->LoadFromData(assetManager->LoadPlainFile("Maps/Map1/map1.nvm"));

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

        SendMessageToClient(player->steamId, &data);
        data.clear();
        data.resize(0);

        UnitIdPacket packet = UnitIdPacket();
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
}

void Client::AddPlayerForNetworkId(int index, LobbyPlayer* player) {
    for (auto tick : all_ticks) {
        SendMessageToClient(player->steamId, &tick);
    }

    CGameObject* pGameObject = new CGameObject();
    CNetworkComponent* pNetComponent = new CNetworkComponent();
    pNetComponent->SetSyncMovement(true);
    pGameObject->AddComponent(new CTransformComponent());
    pGameObject->AddComponent(pNetComponent);

    CMovementComponent* pMoveComponent = new CMovementComponent();
    pGameObject->AddComponent(pMoveComponent);
    pGameObject->AddComponent(new CNavigationComponent());

    std::vector<SpellSlot_t> vecSpells;
    SpellSlot_t spell1;
    spell1.pSpell = new CThunderstrike();
    vecSpells.push_back(spell1);
    CSpellCastComponent* pSpellCast = new CSpellCastComponent(vecSpells);
    pGameObject->AddComponent(pSpellCast);

    CHealthComponent* health = new CHealthComponent(200);
    pGameObject->AddComponent(health);

    pGameObject->AddComponent(new CTeamComponent(Team::TEAM_1));
    pGameObject->AddComponent(new CCharacterComponent(UnitPrefab::STORMCALLER));
    pGameObject->AddComponent(new CBasicAttackComponent());

    AddGameObject(pGameObject);

    UnitIdPacket packet = UnitIdPacket();
    packet.unit_id = pGameObject->GetId();
    std::vector<uint8_t> data;
    packet.Write(&data);
    player->unit = pGameObject->GetId();
    SendMessageToClient(player->steamId, &data);

    /*
    sol::table personTable = m_luaState.script_file("./Scripts/Person/football-person.lua");
    CGameObject* game_object = new CGameObject();
    game_object->current_action_ = nullptr;
    game_object->unit_id = id;
    game_object->nav_agent.UnitId = id;
    game_object->stats.health = personTable.get<int>("hp");
    game_object->stats.max_health = game_object->stats.health;
    game_object->stats.health_regen = personTable.get<float>("hpreg");
    game_object->stats.move_speed = personTable.get<int>("ms");
    game_object->prefab = UnitPrefab::FOOTBALL_PERSON;

    sol::table abilitiesTable = personTable.get<sol::table>("abilities");
    CScriptedSpell* spell1 = new CScriptedSpell(abilitiesTable.get<std::string>(1));
    game_object->spells.push_back(spell1);
    CScriptedSpell* spell2 = new CScriptedSpell(abilitiesTable.get<std::string>(2));
    game_object->spells.push_back(spell2);

    if (index % 2 == 0) {
        game_object->team = Team::TEAM_1;
        game_object->position = { 1000, 0 , -2500};
    }
    else {
        game_object->team = Team::TEAM_2;
        game_object->position = { 4000, 0 , -2500};
    }

    AddGameObject(game_object);
    */
}

void Client::AddGameObject(CGameObject* game_object) {
    GameState.GameObjects.emplace(game_object->GetId(), game_object);
}

void Client::PlayerMoveCommand(PlayerID playerId, float nx, float ny) {
    if (players_.find(playerId) != players_.end()) {
        LobbyPlayer* player = players_.find(playerId)->second;
        CGameObject* person = GameState.GameObjects.find(player->unit)->second;

        GameState.VecEvent.emplace(new CMoveAttemptEvent(
            person->GetId(),
            {nx, 0, ny},
            0
        ));
    }
}

void Client::PlayerStopCommand(PlayerID playerId) {
    if (players_.find(playerId) != players_.end()) {
        LobbyPlayer* player = players_.find(playerId)->second;
        CGameObject* person = GameState.GameObjects.find(player->unit)->second;

        if(CNavigationComponent* pNavComp = person->GetComponent<CNavigationComponent>()) {
            pNavComp->StopNavigation();
            // TODO this belongsn't here
            Vector3 pos = person->GetComponent<CTransformComponent>()->GetPosition();
            GameState.VecEvent.emplace(new CMoveIntentionEvent(person->GetId(), pos, 0));
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

    CGameObject* actor = GetGameObjectById(players_.find(playerId)->second->unit);

    if(actor == nullptr) {
        Logger::FormatErr("Invalid cast command: no game object for player %d", playerId);
        return;
    }

    Logger::FormatMsg("Player %d cast command received", playerId);
    CSpellAttemptCastEvent* pSpellEvent = new CSpellAttemptCastEvent(actor->GetId(), *target_info, spell_slot);
    GameState.VecEvent.emplace(pSpellEvent);
}

void Client::Start() {
    for (auto playerIt : players_) {
        // TODO slot as well!
        AddPlayerForNetworkId(playerIt.second->slot, playerIt.second);
    }
    CGameObject* pDummy = new CGameObject();
    pDummy->AddComponent(new CTransformComponent());
    pDummy->GetComponent<CTransformComponent>()->SetPosition({2000, 0, -2000});
    pDummy->AddComponent(new CMovementComponent());
    pDummy->GetComponent<CMovementComponent>()->ClearTarget();
    pDummy->AddComponent(new CNetworkComponent());
    pDummy->GetComponent<CNetworkComponent>()->SetSyncMovement(true);
    pDummy->AddComponent(new CHealthComponent(100));
    pDummy->AddComponent(new CTeamComponent(Team::TEAM_2));
    pDummy->AddComponent(new CCharacterComponent(UnitPrefab::FOOTBALL_PERSON));
    AddGameObject(pDummy);// TODO need to add him again
}

void Client::CheckCollision(CGameObject* collider) {
    /*
    for (auto go_it : GameState.GameObjects) {
        CGameObject* go = go_it.second;

        if (go->unit_id == collider->unit_id) {
            continue;
        }

        if (TestCollision(
            Circle({ collider->position.x, collider->position.z }, collider->collision_radius),
            Circle({ go->position.x, go->position.z }, 0))
            ) {
            // TODO
            // collider->OnCollision(this, go);
        }
    }
    */

}

std::vector<CGameObject*> Client::GetGameObjectsInArea(Vector2 position, float radius) {
    /*
    std::vector<CGameObject*> vecInArea;

    for (auto go_it : GameState.GameObjects) {
        CGameObject* go = go_it.second;

        if (go && TestCollision(
            Circle(position, radius),
            Circle({ go->position.x, go->position.z }, 0))
            ) {
            vecInArea.push_back(go);
        }
    }
    return vecInArea;
    */

    return {};
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

    for(auto go_it : GameState.GameObjects) {
        CGameObject* go = go_it.second;

        CTransformComponent* pTransform = go->GetComponent<CTransformComponent>();

        if(pTransform == nullptr) {
            continue;
        }

        m_navGrid->GetCellAt(pTransform->GetPosition().x - 25, pTransform->GetPosition().z - 25)->IsOpen = false;
        m_navGrid->GetCellAt(pTransform->GetPosition().x - 25, pTransform->GetPosition().z - 25)->UnitId = go->GetId();
        m_navGrid->GetCellAt(pTransform->GetPosition().x + 25, pTransform->GetPosition().z - 25)->IsOpen = false;
        m_navGrid->GetCellAt(pTransform->GetPosition().x + 25, pTransform->GetPosition().z - 25)->UnitId = go->GetId();
        m_navGrid->GetCellAt(pTransform->GetPosition().x + 25, pTransform->GetPosition().z + 25)->IsOpen = false;
        m_navGrid->GetCellAt(pTransform->GetPosition().x + 25, pTransform->GetPosition().z + 25)->UnitId = go->GetId();
        m_navGrid->GetCellAt(pTransform->GetPosition().x - 25, pTransform->GetPosition().z + 25)->IsOpen = false;
        m_navGrid->GetCellAt(pTransform->GetPosition().x - 25, pTransform->GetPosition().z + 25)->UnitId = go->GetId();
    }
    
    for(ISystem* system : m_vecSystems) {
        system->Update(&GameState, TICKRATE);
    }
    
    for (auto go_it : GameState.GameObjects) {
        CGameObject* go = go_it.second;
        CheckCollision(go);
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

    // TODO why is this stupid
    erase_if(GameState.GameObjects, [this](auto& kv) {
        // if (kv.second->) {
        //    delete kv.second;
        //    return true;
        // }

        return false;
    });
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
    }
}
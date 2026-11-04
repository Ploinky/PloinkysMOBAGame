#include "server-game.h"

void CServerGame::Start(CGameData gameData, NavMesh* pNavMesh) {
    m_gameData = gameData;
    m_gameState.m_pGameData = &m_gameData;

    NavigationMap* pNavMap = new NavigationMap();
    pNavMap->m_pMesh = pNavMesh;
    m_gameState.AddSystem(new CNavigationSystem(pNavMap));
    m_gameState.SetNavMap(pNavMap);
    m_gameState.AddSystem(new CPointsSystem());
    m_gameState.AddSystem(new CRespawnSystem());
    m_gameState.AddSystem(new CAttackSystem());
    m_gameState.AddSystem(new CAiSystem());
    m_gameState.AddSystem(new CInventorySystem());
    m_gameState.AddSystem(new CSpawnSystem(&m_gameData));
    m_gameState.AddSystem(new CTriggerSystem());
    m_gameState.AddSystem(new CMissileSystem());
    m_gameState.AddSystem(new CMovementSystem());
    m_gameState.AddSystem(new CSpellSystem());
    m_gameState.AddSystem(new CDamageSystem());
    m_gameState.AddSystem(new CCleanupSystem());

    // =====================================
    // THIS NEEDS TO GO SOMEWHERE
    // THIS IS A MESS AND MAKES NO SENSE
    // THIS IS GETTING WORSE AND WORSE
    // HOW WILL I EVER MAKE A GAME WHEN I WRITE CODE LIKE THIS
    // WHY AM I TOO STUPID TO MAKE IT WORK
    // IT'S NOT ALL THAT DIFFICULT
    // PLEASE FIGURE IT TF OUT
    
    CTriggerData trigData = m_gameData.mapTriggerData.at("wave_team_1");
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
    // m_gameState.VecTriggers.push_back(trigger);
    // =====================================
    CTriggerData trigData2 = m_gameData.mapTriggerData.at("wave_team_2");
    CTrigger trigger2 = CTrigger();
    for(CTriggerSpawnUnitData spawnTriggerData : trigData2.vecSpawnUnitTriggers) {
        CSpawnUnitTrigger spawnUnitTrigger;
        spawnUnitTrigger.eTeam = spawnTriggerData.eTeam;
        spawnUnitTrigger.idUnitType = spawnTriggerData.idUnitType;
        spawnUnitTrigger.nCount = spawnTriggerData.nCount;
        spawnUnitTrigger.nTime = spawnTriggerData.nTime;
        spawnUnitTrigger.nLastTime = -1;
        spawnUnitTrigger.vec2Position = spawnTriggerData.vec2Position;
        trigger2.vecSpawnUnitTriggers.push_back(spawnUnitTrigger);
    }
    // m_gameState.VecTriggers.push_back(trigger2);
    // =====================================
}

void CServerGame::Input(const IGameEvent& evt) {
    m_gameState.EmitEvent(evt);
    // TODO
    // events "from the outside" (network, demo file, local keyboard and mouse?)
    // probably should be queued
}

void CServerGame::Tick() {
    for(IGameSystem<CServerGameState>* system : m_gameState.AllSystems()) {
        system->Update(&m_gameState, TICKRATE);
    }
    
    for(IGameSystem<CServerGameState>* system : m_gameState.AllSystems()) {
        system->Finalize(&m_gameState);
    }
    m_gameState.CurrentTick++;
}


UnitId CServerGame::AddPlayer() {
    // always present
    UnitId id = m_gameState.SpawnUnit(m_gameData, "stormcaller", Vector2(250, -750)); // TODO position in data;

    BasicAttackComponent_t* pAttack = m_gameState.AddBasicAttack(id, BasicAttackComponent_t());
    pAttack->fRange = 150;
    pAttack->fAttackPoint = 700;
    pAttack->fAttackTime = 1000;
    m_gameState.GetMovement(id)->vec3Target = m_gameState.GetTransform(id)->GetPosition(); // TODO how to make this one call
    m_gameState.GetNavigation(id)->vec3Destination = m_gameState.GetTransform(id)->GetPosition();
    m_gameState.AddRespawn(id);

    return id;
}

CServerGameState* CServerGame::GetState() {
    return &m_gameState;
}
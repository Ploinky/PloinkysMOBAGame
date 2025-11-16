#include <systems/NetworkSystem.h>
#include <components/Components.h>
#include "events/spell-cast-start-event.h"
#include "events/spell-hit-event.h"
#include "events.h"

CNetworkSystem::CNetworkSystem(ServerNetworkManager* pManager) {
    m_pNetworkManager = pManager;

    REGISTER_EVENT_HANDLER(CSpellCastStartEvent, OnSpellCastStart);
    REGISTER_EVENT_HANDLER(CSpellHitEvent, OnSpellhit);
    REGISTER_EVENT_HANDLER(CDeathEvent, OnDeath);
    REGISTER_EVENT_HANDLER(CRespawnEvent, OnRespawn);
    REGISTER_EVENT_HANDLER(CMoveEvent, OnMove);
    REGISTER_EVENT_HANDLER(CMoveIntentionEvent, OnMoveIntention);
    REGISTER_EVENT_HANDLER(CCooldownStartedEvent, OnCooldownStarted);
    REGISTER_EVENT_HANDLER(CAttackStartEvent, OnAttackStart);
    REGISTER_EVENT_HANDLER(CAttackHitEvent, OnAttackHit);
    REGISTER_EVENT_HANDLER(CAttackFinishedEvent, OnAttackFinished);
}

CNetworkSystem::~CNetworkSystem() {
}

void CNetworkSystem::SyncGameState(CServerGameState* pGameState) {
    GameTickPacket tickPck{};
    tickPck.tick = pGameState->CurrentTick++;
    m_pNetworkManager->SendToAllClients(tickPck);

    for(CNetworkComponent& networkComp : pGameState->GetAllNetwork()) {
        if(!networkComp.IsSpawnSynced()) {
            SpawnPacket spawn;
            spawn.unit_type = pGameState->GetCharacter(networkComp.idUnit)->prefab;
            spawn.team = Team::TEAM_1;
            spawn.unit = networkComp.idUnit;
            if(CTransformComponent* pTransform = pGameState->GetTransform(networkComp.idUnit)) {
                spawn.x = pTransform->GetPosition().x;
                spawn.y = pTransform->GetPosition().y;
                spawn.z = pTransform->GetPosition().z;
            } else {
                spawn.x = 0;
                spawn.y = 0;
                spawn.z = 0;
            }
            m_pNetworkManager->SendToAllClients(spawn);

            networkComp.SetSpawnSynced();
        }
        if(CHealthComponent* pHealth = pGameState->GetHealth(networkComp.idUnit)) {
            UnitStatsPacket pck = UnitStatsPacket();
            pck.max_health = pHealth->nMaxHealth;
            pck.health = pHealth->nHealth;
            pck.unit = networkComp.idUnit;
            m_pNetworkManager->SendToAllClients(pck);
        }

        if(CSpellCastComponent* pSpellCast = pGameState->GetSpellCast(networkComp.idUnit)) {
        }
    }

    ScoreUpdatePacket pckScore = ScoreUpdatePacket();
    pckScore.usTeam1Score = pGameState->uTeam1Points;
    pckScore.usTeam2Score = pGameState->uTeam2Points;
    m_pNetworkManager->SendToAllClients(pckScore);
    /*
        CGameObject* pGameObject = entry.second;
        if(pGameObject->NetworkSyncComponent.SyncSpawn) {
            SpawnPacket spawn;
            spawn.unit_type = pGameObject->prefab;
            spawn.team = pGameObject->team;
            spawn.unit = pGameObject->unit_id;
            spawn.x = pGameObject->position.x;
            spawn.y = pGameObject->position.y;
            spawn.z = pGameObject->position.z;
            m_pNetworkManager->SendToAllClients(spawn);
            pGameObject->NetworkSyncComponent.SyncSpawn = false;
        }

        if(pGameObject->NetworkSyncComponent.SyncIdle) {
            UnitIdlePacket idle = UnitIdlePacket();
            idle.unit = pGameObject->unit_id;
            idle.x = pGameObject->position.x;
            idle.y = pGameObject->position.y;
            idle.z = pGameObject->position.z;
            idle.r = pGameObject->rotation.y;
            m_pNetworkManager->SendToAllClients(idle);
            pGameObject->NetworkSyncComponent.SyncIdle = false;
        }

        if(pGameObject->NetworkSyncComponent.SyncTransform) {
            UnitMovePacket move = UnitMovePacket();
            move.unit = pGameObject->unit_id;
            move.x = pGameObject->position.x;
            move.y = pGameObject->position.y;
            move.z = pGameObject->position.z;
            move.r = pGameObject->rotation.y;
            m_pNetworkManager->SendToAllClients(move);
            pGameObject->NetworkSyncComponent.SyncTransform = false;
        }

        if(pGameObject->NetworkSyncComponent.SyncStats) {
            UnitStatsPacket pck = UnitStatsPacket();
            pck.unit = pGameObject->unit_id;
            pck.health = pGameObject->stats.health;
            pck.max_health = pGameObject->stats.max_health;
            m_pNetworkManager->SendToAllClients(pck);
            pGameObject->NetworkSyncComponent.SyncStats = false;
        }

        if(pGameObject->NetworkSyncComponent.SyncAttackStart) {
            AttackStartPacket attack{};
            attack.content.unit = pGameObject->unit_id;
            attack.content.target = pGameObject->BasicAttackInfo.IdTarget;
            m_pNetworkManager->SendToAllClients(attack);
            pGameObject->NetworkSyncComponent.SyncAttackStart = false;
        }

        if(pGameObject->NetworkSyncComponent.SyncSpellCast) {
            // TODO cast start packet?
        }

        if(pGameObject->NetworkSyncComponent.SyncCooldowns) {
            for(int slot = 0; slot < pGameObject->spells.size(); slot++) {
                CScriptedSpell* pSpell = pGameObject->spells.at(slot);
                CooldownPacket cd{};
                cd.unit = pGameObject->unit_id;
                cd.spell_slot = slot;
                cd.cooldown = pSpell->remaining_cooldown;
                cd.total_cooldown = pSpell->cooldown;
                m_pNetworkManager->SendToAllClients(cd);
                // TODO only send packets where the cooldown has actually now changed!
            }
            pGameObject->NetworkSyncComponent.SyncCooldowns = false;
        }

        if(pGameObject->NetworkSyncComponent.SyncDespawn) {
            DespawnPacket pck = DespawnPacket();
            pck.unit = pGameObject->unit_id;
            m_pNetworkManager->SendToAllClients(pck);
            // TODO since this flag also decides which objects are deleted, we do NOT clear it here
        }
        
    }
    */
}

void CNetworkSystem::OnSpellCastStart(CServerGameState* pGameState, CSpellCastStartEvent* pStartCastEvt) {
    SpellCastStartPacket pck = SpellCastStartPacket();
    pck.unit = pStartCastEvt->pCtx->idCaster;
    pck.idTarget = pStartCastEvt->pCtx->idTarget;
    m_pNetworkManager->SendToAllClients(pck);
}

void CNetworkSystem::OnSpellhit(CServerGameState* pGameState, CSpellHitEvent* pHitEvt) {
    SpellHitPacket pck = SpellHitPacket();
    pck.unit = pHitEvt->idTarget;
    pck.spell = pHitEvt->strSpell;
    m_pNetworkManager->SendToAllClients(pck);
}

void CNetworkSystem::OnDeath(CServerGameState* pGameState, CDeathEvent* pEvt) {
    CUnitDeathPacket pck = CUnitDeathPacket();
    pck.idUnit = pEvt->idTarget;
    m_pNetworkManager->SendToAllClients(pck);
}

void CNetworkSystem::OnRespawn(CServerGameState* pGameState, CRespawnEvent* pEvt) {
    CUnitRespawnPacket pck = CUnitRespawnPacket();
    pck.idUnit = pEvt->idTarget;
    m_pNetworkManager->SendToAllClients(pck);
}

void CNetworkSystem::OnMove(CServerGameState* pGameState, CMoveEvent* pEvt) {
    UnitMovePacket pck = UnitMovePacket();
    pck.unit = pEvt->idUnit;
    pck.x = pEvt->vec3Position.x;
    pck.y = pEvt->vec3Position.y;
    pck.z = pEvt->vec3Position.z;
    pck.r = pEvt->fRotation;
    m_pNetworkManager->SendToAllClients(pck);
}

void CNetworkSystem::OnMoveIntention(CServerGameState* pGameState, CMoveIntentionEvent* pEvt) {
    UnitMoveIntentionPacket moveInt = UnitMoveIntentionPacket();
    moveInt.unit = pEvt->idUnit;
    moveInt.x = pEvt->vec3Position.x;
    moveInt.y = pEvt->vec3Position.y;
    moveInt.z = pEvt->vec3Position.z;
    m_pNetworkManager->SendToAllClients(moveInt);
}

void CNetworkSystem::OnCooldownStarted(CServerGameState* pGameState, CCooldownStartedEvent* pEvt) {
    CooldownPacket pck = CooldownPacket();
    pck.cooldown = pEvt->fCooldown;
    pck.unit = pEvt->idUnit;
    pck.spell_slot = pEvt->nSpellIndex;
    pck.total_cooldown = pEvt->fCooldown;
    m_pNetworkManager->SendToAllClients(pck);
}

void CNetworkSystem::OnAttackStart(CServerGameState* pGameState, CAttackStartEvent* pEvt) {
    attack_start_pck_t pckData {};
    pckData.unit = pEvt->idAttacker;
    pckData.target = pEvt->idTarget;

    AttackStartPacket pck = AttackStartPacket();
    pck.content = pckData;
    m_pNetworkManager->SendToAllClients(pck);
}

void CNetworkSystem::OnAttackHit(CServerGameState* pGameState, CAttackHitEvent* pEvt) {

}


void CNetworkSystem::OnAttackFinished(CServerGameState* pGameState, CAttackFinishedEvent* pEvt) {
    attack_finished_pck_t pckData {};
    pckData.unit = pEvt->idAttacker;

    CAttackFinishedPacket pck = CAttackFinishedPacket();
    pck.content = pckData;
    m_pNetworkManager->SendToAllClients(pck);
}
#include "../systems/NetworkSystem.h"
#include "../components/Components.h"
#include "../events/spell-cast-start-event.h"
#include "../events/spell-hit-event.h"
#include "../events.h"

CNetworkSystem::CNetworkSystem(ServerNetworkManager* pManager) {
    m_pNetworkManager = pManager;
}

CNetworkSystem::~CNetworkSystem() {
}

void CNetworkSystem::SyncGameState(CServerGameState* pGameState) {
    GameTickPacket tickPck{};
    tickPck.tick = pGameState->CurrentTick;
    m_pNetworkManager->SendToAllClients(tickPck);

    for(auto& [id, networkComp] : pGameState->GetAllNetwork()) {
        if(!networkComp.IsSpawnSynced()) {
            SpawnPacket spawn;
            spawn.strEntId = pGameState->GetCharacter(id)->prefab;

            if (TeamComponent_t* pTeam = pGameState->GetTeam(id)) {
                spawn.team = pTeam->eTeam;
            }
            spawn.unit = id;
            if(TransformComponent_t* pTransform = pGameState->GetTransform(id)) {
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
        if(HealthComponent_t* pHealth = pGameState->GetHealth(id)) {
            UnitStatsPacket pck = UnitStatsPacket();
            pck.max_health = pHealth->nMaxHealth;
            pck.health = pHealth->nHealth;
            pck.unit = id;
            m_pNetworkManager->SendToAllClients(pck);
        }

        if(SpellCastComponent_t* pSpellCast = pGameState->GetSpellCast(id)) {
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

void CNetworkSystem::OnSpellCastStart(CServerGameState* pGameState, const CSpellCastStartEvent& evt) {
    SpellCastStartPacket pck = SpellCastStartPacket();
    pck.unit = evt.pCtx->idCaster;
    pck.idTarget = evt.pCtx->idTarget;
    m_pNetworkManager->SendToAllClients(pck);
}

void CNetworkSystem::OnEntityIdle(CServerGameState* pGameState, const CEntityIdleEvent& evt) {
    UnitIdlePacket pck = UnitIdlePacket();
    pck.unit = evt.idTarget;

    if (TransformComponent_t* pTransform = pGameState->GetTransform(evt.idTarget)) {
        pck.x = pTransform->GetPosition().x;
        pck.y = pTransform->GetPosition().y;
        pck.z = pTransform->GetPosition().z;
        pck.r = pTransform->GetRotation().y;
    } else {
        pck.x = 0.0f;
        pck.y = 0.0f;
        pck.z = 0.0f;
        pck.r = 0.0f;
    }
    m_pNetworkManager->SendToAllClients(pck);
}

void CNetworkSystem::OnSpellhit(CServerGameState* pGameState, const CSpellHitEvent& evt) {
    SpellHitPacket pck = SpellHitPacket();
    pck.unit = evt.idTarget;
    pck.spell = evt.strSpell;
    m_pNetworkManager->SendToAllClients(pck);
}

void CNetworkSystem::OnDeath(CServerGameState* pGameState, const CDeathEvent& evt) {
    CUnitDeathPacket pck = CUnitDeathPacket();
    pck.idUnit = evt.idTarget;
    m_pNetworkManager->SendToAllClients(pck);
}

void CNetworkSystem::OnRespawn(CServerGameState* pGameState, const CRespawnEvent& evt) {
    CUnitRespawnPacket pck = CUnitRespawnPacket();
    pck.idUnit = evt.idTarget;
    m_pNetworkManager->SendToAllClients(pck);
}

void CNetworkSystem::OnMove(CServerGameState* pGameState, const CMoveEvent& evt) {
    UnitMovePacket pck = UnitMovePacket();
    pck.unit = evt.idUnit;
    pck.x = evt.vec3Position.x;
    pck.y = evt.vec3Position.y;
    pck.z = evt.vec3Position.z;
    pck.r = evt.fRotation;
    m_pNetworkManager->SendToAllClients(pck);
}

void CNetworkSystem::OnMoveIntention(CServerGameState* pGameState, const CMoveIntentionEvent& evt) {
    UnitMoveIntentionPacket moveInt = UnitMoveIntentionPacket();
    moveInt.unit = evt.idUnit;
    moveInt.x = evt.vec3Position.x;
    moveInt.y = evt.vec3Position.y;
    moveInt.z = evt.vec3Position.z;
    m_pNetworkManager->SendToAllClients(moveInt);
}

void CNetworkSystem::OnCooldownStarted(CServerGameState* pGameState, const CCooldownStartedEvent& evt) {
    CooldownPacket pck = CooldownPacket();
    pck.cooldown = evt.fCooldown;
    pck.unit = evt.idUnit;
    pck.spell_slot = evt.nSpellIndex;
    pck.total_cooldown = evt.fCooldown;
    m_pNetworkManager->SendToAllClients(pck);
}

void CNetworkSystem::OnAttackStart(CServerGameState* pGameState, const CAttackStartEvent& evt) {
    attack_start_pck_t pckData {};
    pckData.unit = evt.idAttacker;
    pckData.target = evt.idTarget;

    AttackStartPacket pck = AttackStartPacket();
    pck.content = pckData;
    m_pNetworkManager->SendToAllClients(pck);
}

void CNetworkSystem::OnAttackHit(CServerGameState* pGameState, const CAttackHitEvent& evt) {

}


void CNetworkSystem::OnAttackFinished(CServerGameState* pGameState, const CAttackFinishedEvent& evt) {
    attack_finished_pck_t pckData {};
    pckData.unit = evt.idAttacker;

    CAttackFinishedPacket pck = CAttackFinishedPacket();
    pck.content = pckData;
    m_pNetworkManager->SendToAllClients(pck);
}

void CNetworkSystem::OnPickedUp(CServerGameState* pGameState, const CPickedUpEvent& evt) {
    CPickedUpEntityPacket pck;
    pck.idUnit = evt.idUnit;
    pck.idPickedUpUnit = evt.idTargetUnit;

    m_pNetworkManager->SendToAllClients(pck);
}

void CNetworkSystem::OnEntityDespawn(CServerGameState* pGameState,  const CDespawnEvent& evt) {
    DespawnPacket pck;
    pck.unit = evt.idTarget;
    m_pNetworkManager->SendToAllClients(pck);
}
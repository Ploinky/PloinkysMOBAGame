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
}

CNetworkSystem::~CNetworkSystem() {
}

void CNetworkSystem::SyncGameState(CGameState* pGameState) {
    GameTickPacket tickPck{};
    tickPck.tick = pGameState->CurrentTick++;
    m_pNetworkManager->SendToAllClients(tickPck);

    for(std::pair<unsigned int, CGameObject*> entry : pGameState->GameObjects) {
        CGameObject* pGameObject = entry.second;
        CNetworkComponent* pNetComponent = pGameObject->GetComponent<CNetworkComponent>();
        if(pNetComponent == nullptr) {
            continue;
        }

        if(!pNetComponent->IsSpawnSynced()) {
            SpawnPacket spawn;
            spawn.unit_type = UnitPrefab::FOOTBALL_PERSON;
            spawn.team = Team::TEAM_1;
            spawn.unit = pGameObject->GetId();
            if(CTransformComponent* pTransform = pGameObject->GetComponent<CTransformComponent>()) {
                spawn.x = pTransform->GetPosition().x;
                spawn.y = pTransform->GetPosition().y;
                spawn.z = pTransform->GetPosition().z;
            } else {
                spawn.x = 0;
                spawn.y = 0;
                spawn.z = 0;
            }
            m_pNetworkManager->SendToAllClients(spawn);

            pNetComponent->SetSpawnSynced();
        }

        if(pNetComponent->SyncMovement()) {
             if(CTransformComponent* pTransform = pGameObject->GetComponent<CTransformComponent>()) {
                if(CMovementComponent* pMove = pGameObject->GetComponent<CMovementComponent>()) {
                    if(pMove->GetTarget() != pTransform->GetPosition()) {
                        UnitMovePacket move = UnitMovePacket();
                        move.unit = pGameObject->GetId();
                        move.x = pTransform->GetPosition().x;
                        move.y = pTransform->GetPosition().y;
                        move.z = pTransform->GetPosition().z;
                        move.r = pTransform->GetRotation().y;
                        m_pNetworkManager->SendToAllClients(move);
                    }

                    UnitMoveIntentionPacket moveInt = UnitMoveIntentionPacket();
                    moveInt.unit = pGameObject->GetId();
                    moveInt.x = pMove->GetTarget().x;
                    moveInt.y = pMove->GetTarget().y;
                    moveInt.z = pMove->GetTarget().z;
                    m_pNetworkManager->SendToAllClients(moveInt);
                } else {
                    UnitMovePacket move = UnitMovePacket();
                    move.unit = pGameObject->GetId();
                    move.x = pTransform->GetPosition().x;
                    move.y = pTransform->GetPosition().y;
                    move.z = pTransform->GetPosition().z;
                    move.r = pTransform->GetRotation().y;
                    m_pNetworkManager->SendToAllClients(move);
                }
            }
        }

        if(CHealthComponent* pHealth = pGameObject->GetComponent<CHealthComponent>()) {
            UnitStatsPacket pck = UnitStatsPacket();
            pck.max_health = pHealth->nMaxHealth;
            pck.health = pHealth->nHealth;
            pck.unit = pGameObject->GetId();
            m_pNetworkManager->SendToAllClients(pck);
        }

        if(CSpellCastComponent* pSpellCast = pGameObject->GetComponent<CSpellCastComponent>()) {
        }
    }
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

void CNetworkSystem::OnSpellCastStart(CGameState* pGameState, CSpellCastStartEvent* pStartCastEvt) {
    SpellCastStartPacket pck = SpellCastStartPacket();
    pck.unit = pStartCastEvt->pCtx->idCaster;
    m_pNetworkManager->SendToAllClients(pck);
}

void CNetworkSystem::OnSpellhit(CGameState* pGameState, CSpellHitEvent* pHitEvt) {
    SpellHitPacket pck = SpellHitPacket();
    pck.unit = pHitEvt->idTarget;
    pck.spell = pHitEvt->strSpell;
    m_pNetworkManager->SendToAllClients(pck);
}

void CNetworkSystem::OnDeath(CGameState* pGameState, CDeathEvent* pEvt) {
    CUnitDeathPacket pck = CUnitDeathPacket();
    pck.idUnit = pEvt->idTarget;
    m_pNetworkManager->SendToAllClients(pck);
}

void CNetworkSystem::OnRespawn(CGameState* pGameState, CRespawnEvent* pEvt) {
    CUnitRespawnPacket pck = CUnitRespawnPacket();
    pck.idUnit = pEvt->idTarget;
    m_pNetworkManager->SendToAllClients(pck);
}
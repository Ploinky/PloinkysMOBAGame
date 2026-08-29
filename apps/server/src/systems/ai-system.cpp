#include "ai-system.h"

#include "../game/server-game-state.h"
#include "../GameObject.h"
#include "../components/Components.h"

CAiSystem::CAiSystem() {
};

void CAiSystem::Update(CServerGameState* pGameState, float fDelta) {
    IntentComponent_t* pIntentComp = nullptr;
    TransformComponent_t* pTransform = nullptr;
    TeamComponent_t* pTeam = nullptr;
    TeamComponent_t* pOtherTeam = nullptr;
    std::vector<UnitId> otherUnitsInArea = {};

    for(auto& [id, aiComp] : pGameState->GetAllAi()) {
        switch(aiComp.eType) {
            case EAiType::MINION:
                pIntentComp = pGameState->GetIntent(id);
                pTransform = pGameState->GetTransform(id);
                pTeam = pGameState->GetTeam(id);

                if(pIntentComp == nullptr) {
                    continue;
                }

                otherUnitsInArea = pGameState->GetUnitsInAra(Vector2(pTransform->GetPosition().x, pTransform->GetPosition().y), 600);

                if (pIntentComp->eType != EIntentType::ATTACK && otherUnitsInArea.size() > 0) {
                    for (UnitId idOther : otherUnitsInArea) {
                        if (HealthComponent_t* pOtherHealth = pGameState->GetHealth(idOther)) {
                            if (pOtherHealth->bIsDead) {
                                continue;
                            }
                        }
                        pOtherTeam = pGameState->GetTeam(idOther);

                        if (pOtherTeam->eTeam != pTeam->eTeam) {
                            pGameState->EmitEvent(CAttackIntentionEvent(id, idOther));
                            break;
                        }
                    }
                }

                if(pIntentComp->eType == EIntentType::NONE && !aiComp.vecWaypoints.empty()) {
                    Vector3 vec3Wp = aiComp.vecWaypoints.front();
                    pIntentComp->eType = EIntentType::MOVE;
                    pIntentComp->vec3Target = vec3Wp;
                }
                break;
            default:
                continue;
        }
    }
}

void CAiSystem::OnNavDest(CServerGameState* pGameState, const CNavDestEvent& evt) {
    AiComponent_t* pAiComp = pGameState->GetAi(evt.idUnit);

    if(pAiComp == nullptr) {
        return;
    }

    // TODO magic number
    if(!pAiComp->vecWaypoints.empty() && (pAiComp->vecWaypoints.front() - evt.vec3Target).Length() < 500) {
        pAiComp->vecWaypoints.pop_front();
    }
}

void CAiSystem::OnUnitDeath(CServerGameState* pGameState, const CDeathEvent& evt) {
    for (auto& [id, intent] : pGameState->GetAllIntent()) {
        if (intent.eType == EIntentType::ATTACK || intent.eType == EIntentType::CAST_SPELL) {
            if (intent.idTarget == evt.idTarget) {
                intent.eType = EIntentType::NONE;
                intent.idTarget = UNIT_ID_NONE;
                intent.vec3Target = Vector3::ZERO;
            }
        }
    }
}
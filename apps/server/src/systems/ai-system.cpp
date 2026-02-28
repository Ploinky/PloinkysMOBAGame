#include "ai-system.h"

#include "game/server-game-state.h"
#include "GameObject.h"
#include "components/Components.h"

CAiSystem::CAiSystem() {
};

void CAiSystem::Update(CServerGameState* pGameState, float fDelta) {
    CIntentComponent* pIntentComp = nullptr;
    for(CAiComponent& aiComp : pGameState->GetAllAi()) {
        switch(aiComp.eType) {
            case EAiType::MINION:
                pIntentComp = pGameState->GetIntent(aiComp.idUnit);
                if(pIntentComp == nullptr) {
                    continue;
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

void CAiSystem::OnNavDest(CServerGameState* pGameState, CNavDestEvent* pNavDestEvt) {
    CAiComponent* pAiComp = pGameState->GetAi(pNavDestEvt->idUnit);

    if(pAiComp == nullptr) {
        return;
    }

    // TODO magic number
    if(!pAiComp->vecWaypoints.empty() && (pAiComp->vecWaypoints.front() - pNavDestEvt->vec3Target).Length() < 500) {
        pAiComp->vecWaypoints.pop_front();
    }
}
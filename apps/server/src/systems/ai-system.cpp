#include "ai-system.h"

#include "game/server-game-state.h"
#include "GameObject.h"
#include "components/Components.h"

void CAiSystem::Update(CServerGameState* pGameState, float fDelta) {
    CIntentComponent* pIntentComp = nullptr;
    for(CAiComponent& aiComp : pGameState->GetAllAi()) {
        switch(aiComp.eType) {
            case EAiType::MINION:
                pIntentComp = pGameState->GetIntent(aiComp.idUnit);
                if(pIntentComp == nullptr) {
                    continue;
                }

                if(pIntentComp->eType == EIntentType::NONE) {
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
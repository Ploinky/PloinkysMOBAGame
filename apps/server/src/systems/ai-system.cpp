#include "ai-system.h"

#include "game/server-game-state.h"
#include "GameObject.h"
#include "components/Components.h"

void CAiSystem::Update(CServerGameState* pGameState, float fDelta) {
    for(auto goPair : pGameState->GameObjects) {
        CGameObject* go = goPair.second;

        CAiComponent* aiComp = go->GetComponent<CAiComponent>();

        if(aiComp == nullptr) {
            continue;
        }

        CIntentComponent* pIntentComp = nullptr;
        switch(aiComp->eType) {
            case EAiType::MINION:
                pIntentComp = go->GetComponent<CIntentComponent>();
                if(pIntentComp == nullptr) {
                    return;
                }

                if(pIntentComp->eType == EIntentType::NONE) {
                    Vector3 vec3Wp = aiComp->vecWaypoints.front();
                    pIntentComp->eType = EIntentType::MOVE;
                    pIntentComp->vec3Target = vec3Wp;
                }
                break;
            default:
                continue;
        }
    }
}
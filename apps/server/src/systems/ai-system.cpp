#include "ai-system.h"

#include "game/server-game-state.h"
#include "GameObject.h"
#include "components/Components.h"

CAiSystem::CAiSystem() {
    REGISTER_EVENT_HANDLER(CMoveEvent, OnMove);
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


void CAiSystem::OnMove(CServerGameState* pGameState, CMoveEvent* pMoveEvent) {
    CAiComponent* aiComp = pGameState->GetAi(pMoveEvent->idUnit);
    CTransformComponent* transform = pGameState->GetTransform(pMoveEvent->idUnit);

    if(aiComp == nullptr || aiComp->vecWaypoints.empty()) {
        return;
    }

    Vector3 vec3Wp = aiComp->vecWaypoints.front();
    // TODO fix this right
    if((Vector2(vec3Wp.x, vec3Wp.z) - Vector2(transform->GetPosition().x, transform->GetPosition().z)).Length() < 200) {
        std::vector<Vector3> newWp;
        for(int i = 1; i < aiComp->vecWaypoints.size(); i++) {
            newWp.push_back(aiComp->vecWaypoints.at(i));
        }

        aiComp->vecWaypoints = newWp;
    }
}
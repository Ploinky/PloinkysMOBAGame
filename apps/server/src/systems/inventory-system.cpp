#include "inventory-system.h"

#include "game/server-game-state.h"
#include "events.h"
#include "GameObject.h"
#include "components/Components.h"


CInventorySystem::CInventorySystem() {
    REGISTER_EVENT_HANDLER(CPickUpAttemptEvent, OnPickUpEntityAttempt);
}

void CInventorySystem::Update(CServerGameState* pGameState, float fDelta) { 
}

void CInventorySystem::Finalize(CServerGameState* pGameState) {
}

void CInventorySystem::OnPickUpEntityAttempt(CServerGameState* pGameState, CPickUpAttemptEvent* pPickUpEvt) {
    CInventoryComponent* pInventory = pGameState->GetInventory(pPickUpEvt->idUnit);

    if(pInventory == nullptr) {
        Logger::FormatErr("Invalid pick up event: unit <%d> does not have an inventory", pPickUpEvt->idUnit);
        return;
    }

    CPickupableComponent* pPickupable = pGameState->GetPickupable(pPickUpEvt->idTargetUnit);

    if(pPickupable == nullptr) {
        Logger::FormatErr("Invalid pick up event: unit <%d> is not pickupable", pPickUpEvt->idTargetUnit);
        return;
    }

    pInventory->vecEntitiesInInventory.push_back(pPickUpEvt->idTargetUnit);

    CPickedUpEvent* pEvt = new CPickedUpEvent(pPickUpEvt->idUnit, pPickUpEvt->idTargetUnit);
    pGameState->VecEvent.emplace(pEvt);
}
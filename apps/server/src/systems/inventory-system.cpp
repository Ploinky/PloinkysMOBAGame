#include "inventory-system.h"

#include "game/server-game-state.h"
#include "events.h"
#include "GameObject.h"
#include "components/Components.h"


CInventorySystem::CInventorySystem() {
}

void CInventorySystem::Update(CServerGameState* pGameState, float fDelta) { 
}

void CInventorySystem::Finalize(CServerGameState* pGameState) {
}

void CInventorySystem::OnPickUpEntityAttempt(CServerGameState* pGameState, CPickUpAttemptEvent* pPickUpEvt) {
    InventoryComponent_t* pInventory = pGameState->GetInventory(pPickUpEvt->idUnit);

    if(pInventory == nullptr) {
        Logger::FormatErr("Invalid pick up event: unit <%d> does not have an inventory", pPickUpEvt->idUnit);
        return;
    }

    PickupableComponent_t* pPickupable = pGameState->GetPickupable(pPickUpEvt->idTargetUnit);

    if(pPickupable == nullptr) {
        Logger::FormatErr("Invalid pick up event: unit <%d> is not pickupable", pPickUpEvt->idTargetUnit);
        return;
    }

    pInventory->vecEntitiesInInventory.push_back(pPickUpEvt->idTargetUnit);

    CPickedUpEvent* pEvt = new CPickedUpEvent(pPickUpEvt->idUnit, pPickUpEvt->idTargetUnit);
    pGameState->EmitEvent(pEvt);
}

void CInventorySystem::OnUseEntityAttempt(CServerGameState* pGameState, CUseEntityAttemptEvent* pEvt) {
    InventoryComponent_t* pInventory = pGameState->GetInventory(pEvt->idUser);

    if(pInventory == nullptr) {
        Logger::FormatErr("Invalid pick up event: unit <%d> does not have an inventory", pEvt->idUser);
        return;
    }

    UseableComponent_t* pUseable = pGameState->GetUseable(pEvt->idEntity);

    if(pUseable == nullptr) {
        Logger::FormatErr("Invalid pick up event: unit <%d> is not useable", pEvt->idEntity);
        return;
    }

    // TODO check if it's even there?
    CUseEntityEvent* pUseEvent = new CUseEntityEvent();
    pUseEvent->idUser = pEvt->idUser;
    pUseEvent->idEntity = pEvt->idEntity;
    pUseEvent->x = pEvt->x;
    pUseEvent->y = pEvt->y;
    pGameState->EmitEvent(pUseEvent);
}
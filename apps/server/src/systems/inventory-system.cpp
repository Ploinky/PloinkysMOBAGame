#include "inventory-system.h"

#include "../game/server-game-state.h"
#include "../events.h"
#include "../GameObject.h"
#include "../components/Components.h"


CInventorySystem::CInventorySystem() {
}

void CInventorySystem::Update(CServerGameState* pGameState, float fDelta) { 
}

void CInventorySystem::Finalize(CServerGameState* pGameState) {
}

void CInventorySystem::OnPickUpEntityAttempt(CServerGameState* pGameState, const CPickUpAttemptEvent& evt) {
    InventoryComponent_t* pInventory = pGameState->GetInventory(evt.idUnit);

    if(pInventory == nullptr) {
        Logger::FormatErr("Invalid pick up event: unit <%d> does not have an inventory", evt.idUnit);
        return;
    }

    PickupableComponent_t* pPickupable = pGameState->GetPickupable(evt.idTargetUnit);

    if(pPickupable == nullptr) {
        Logger::FormatErr("Invalid pick up event: unit <%d> is not pickupable", evt.idTargetUnit);
        return;
    }

    pInventory->vecEntitiesInInventory.push_back(evt.idTargetUnit);

    pGameState->EmitEvent(CPickedUpEvent(evt.idUnit, evt.idTargetUnit));
}

void CInventorySystem::OnUseEntityAttempt(CServerGameState* pGameState, const CUseEntityAttemptEvent& evt) {
    InventoryComponent_t* pInventory = pGameState->GetInventory(evt.idUser);

    if(pInventory == nullptr) {
        Logger::FormatErr("Invalid pick up event: unit <%d> does not have an inventory", evt.idUser);
        return;
    }

    UseableComponent_t* pUseable = pGameState->GetUseable(evt.idEntity);

    if(pUseable == nullptr) {
        Logger::FormatErr("Invalid pick up event: unit <%d> is not useable", evt.idEntity);
        return;
    }

    // TODO check if it's even there?
    CUseEntityEvent useEvent = CUseEntityEvent();
    useEvent.idUser = evt.idUser;
    useEvent.idEntity = evt.idEntity;
    useEvent.x = evt.x;
    useEvent.y = evt.y;
    pGameState->EmitEvent(useEvent);
}
#include "server-game-state.h"

#include "GameObject.h"

void CServerGameState::SetNavMap(NavigationMap* pNavMap) {
    m_pNavMap = pNavMap;
}

NavigationMap* CServerGameState::GetNavMap() const {
    return m_pNavMap;
}

UnitId CServerGameState::CreateEntity() {
    return CurrentUnitId++;
}

UnitId CServerGameState::SpawnUnit(const CGameData& gameData, std::string strId, Vector2 vec2Pos) {
    UnitId idSpawnedUnit = SpawnUnit(gameData, strId);

    if(CTransformComponent* pTransform = GetTransform(idSpawnedUnit)) {
        pTransform->SetPosition({vec2Pos.x, 0, vec2Pos.y});
    }

    return idSpawnedUnit;
}

UnitId CServerGameState::SpawnUnit(const CGameData& gameData, std::string strId) {
    CCharacterData entityData = gameData.mapCharacterData.at(strId);

    UnitId id = CurrentUnitId++;

    // ==================== CHARACTER COMPONENT ====================
    AddCharacter(id, CCharacterComponent(entityData.strId));

    // ==================== SPELL CAST COMPONENT ====================
    if(!entityData.mapAbilityIds.empty()) {
        CSpellCastComponent* spellComp = AddSpellCast(id);

        for(auto it : entityData.mapAbilityIds) {
            if(spellComp->vecSpellSlots.size() < it.first + 1) {
                spellComp->vecSpellSlots.resize(it.first + 1);
            }

            SpellSlot_t spellSlot{};
            spellSlot.data = gameData.mapAbilityData.at(it.second);
            spellSlot.fCooldownRemaining = 0.0f;
            spellSlot.level = 0.0f;
            GetSpellCast(id)->vecSpellSlots[it.first] = spellSlot;
        }
    }

    // ==================== TRANSFORM COMPONENT ====================
    if(entityData.optTransformData.has_value()) {
        AddTransform(id);
        // TODO read out collision radius
    }

    // ==================== NETWORK COMPONENT ====================
    if(entityData.optNetworkData.has_value()) {
        CNetworkComponent* netComp = AddNetwork(id);
        netComp->SetSyncMovement(entityData.optNetworkData.value().bSyncMovement);
    }

    // ==================== MOVEMENT COMPONENT ====================
    if(entityData.optMovementData.has_value()) {
        CMovementComponent* moveComp = AddMovement(id);
        moveComp->fSpeed = entityData.optMovementData.value().nSpeed;
    }

    // ==================== HEALTH COMPONENT ====================
    if(entityData.optHealthData.has_value()) {
        CHealthComponent* healthComp = AddHealth(id);
        healthComp->nMaxHealth = entityData.optHealthData.value().nMaxHealth;
    }

    // ==================== NAVIGATION COMPONENT ====================
    if(entityData.optNavigationData.has_value()) {
        CNavigationComponent* pNav = AddNavigation(id);
        pNav->pNavGridAgent->UnitId = id;

    }

    // ==================== INTENT COMPONENT ====================
    if(entityData.optIntentData.has_value()) {
        CIntentComponent* pIntent = AddIntent(id);
        pIntent->eType = EIntentType::NONE;
    }

    if(entityData.optPickupableData.has_value()) {
        AddPickupable(id);
    }

    if(entityData.optUseableData.has_value()) {
        CUseableComponent* pUseableComponent = AddUseable(id);
        pUseableComponent->nUses = entityData.optUseableData.value().nUses;
        pUseableComponent->abilityData = gameData.mapAbilityData.at(entityData.optUseableData.value().strAbilityId);
    }

    if(entityData.optInventoryData.has_value()) {
        AddInventory(id);
    }

    if(entityData.optAIData.has_value()) {
        CAiComponent* pAi = AddAi(id);

        if(entityData.optAIData.value().idType == "minion") {
            pAi->eType = EAiType::MINION;
        } else {
            pAi->eType = EAiType::MINION;
        }

        pAi->vecWaypoints.push_back({1200, 0, -750});

        pAi->idUnit = id;
    }

    return id;
}
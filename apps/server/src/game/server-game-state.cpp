#include "server-game-state.h"

#include "GameObject.h"

std::vector<CGameObject*> CServerGameState::GetGameObjectsInArea(Vector3 vec3Center, float fRadius) const {
    throw std::runtime_error("implement me");
    return {};
}

CGameObject* CServerGameState::FindGameObjectById(UnitId idUnit) const {
    std::map<UnitId, CGameObject*>::const_iterator entry = GameObjects.find(idUnit);

    if(entry == GameObjects.end()) {
        return nullptr;
    }

    return entry->second;
}

void CServerGameState::SetNavMap(NavigationMap* pNavMap) {
    m_pNavMap = pNavMap;
}

NavigationMap* CServerGameState::GetNavMap() const {
    return m_pNavMap;
}

UnitId CServerGameState::SpawnUnit(const CGameData& gameData, std::string strId) {
    CCharacterData entityData = gameData.mapCharacterData.at(strId);

    CGameObject* go = new CGameObject();
    this->GameObjects.emplace(go->GetId(), go);
    UnitId id = go->GetId();

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
        AddNavigation(id);
    }

    return id;
}
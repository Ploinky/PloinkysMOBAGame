#include "spell-cast-api.h"

#include "GameObject.h"
#include "components/Components.h"
#include "events.h"
#include "game/server-game-state.h"

CSpellCastApi::CSpellCastApi(CServerGameState* pGameState) {
    m_pGameState = pGameState;
}

void CSpellCastApi::ApplyDamage(UnitId idSource, UnitId idTarget, float fDamage) {
    m_pGameState->EmitEvent(CDamageEvent(idSource, idTarget, fDamage));
}

void CSpellCastApi::ApplyHeal(UnitId idSource, UnitId idTarget, float fDamage) {
    m_pGameState->EmitEvent(CHealEvent(idSource, idTarget, fDamage));
}

void CSpellCastApi::SpawnEntity(UnitId idSpawner, std::string strTemplateId, Vector2 vec2Destination) {
    bool isMissile = m_pGameState->m_pGameData->mapCharacterData.at(strTemplateId).optMissileData.has_value();

    Vector2 spawnLocation = vec2Destination;

    if (isMissile) {
        Vector3 vec3Spawn = m_pGameState->GetTransform(idSpawner)->GetPosition();
        spawnLocation = Vector2(vec3Spawn.x, vec3Spawn.z);
    }

    UnitId idUnit = m_pGameState->SpawnUnit(*m_pGameState->m_pGameData, strTemplateId, spawnLocation);

    if (MissileComponent_t* pMissile = m_pGameState->GetMissile(idUnit)) {
        pMissile->idOwner = idSpawner;
        pMissile->vec3Target = Vector3(vec2Destination.x, 0, vec2Destination.y);

        if (TeamComponent_t* pSpawnerTeam = m_pGameState->GetTeam(idSpawner)) {
            m_pGameState->AddTeam(idUnit)->eTeam = pSpawnerTeam->eTeam;
        }
    }
}
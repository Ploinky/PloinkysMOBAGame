#pragma once

#include "system.h"
#include "events.h"
#include "common/data/game-data.h"

class CSpawnSystem : public ISystem {
public:
    CSpawnSystem(const CGameData* pGameData);

    virtual void Update(CServerGameState* pGameState, float fDelta) override;

    void OnUnitSpawn(CServerGameState* pGameState, CUnitSpawnEvent* pEvt);

private:
    const CGameData* m_pGameData;
};
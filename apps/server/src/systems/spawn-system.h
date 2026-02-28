#pragma once

#include "common/game/game-system.h"
#include "common/data/game-data.h"
#include "events.h"

class CSpawnSystem : public IGameSystem<CServerGameState> {
public:
    CSpawnSystem(const CGameData* pGameData);

    virtual void Update(CServerGameState* pGameState, float fDelta) override;

    REGISTER_EVENT_HANDLER(CSpawnSystem, CUnitSpawnEvent, OnUnitSpawn)

private:
    const CGameData* m_pGameData;
};
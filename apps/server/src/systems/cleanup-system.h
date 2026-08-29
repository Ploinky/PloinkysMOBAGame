#pragma once

#include <common/game/game-system.h>

#include "../events.h"
#include "../components/Components.h"

class CServerGameState;

class CCleanupSystem : public IGameSystem<CServerGameState> {
public:
    virtual void Update(CServerGameState* pGameState, float fDelta) override;

    REGISTER_EVENT_HANDLER(CCleanupSystem, CDeathEvent, OnUnitDied)

private:
    void CleanupUnit(CServerGameState* pGameState, UnitId id);

    std::vector<UnitId> m_vecUnitsToCleanUp;
};
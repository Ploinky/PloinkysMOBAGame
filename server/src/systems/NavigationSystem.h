#pragma once

#include <GameState.h>
#include "system.h"
#include "events.h"

class NavigationMap;

class CNavigationSystem : public ISystem{
public:
    CNavigationSystem(NavigationMap* pMap);

    virtual void Update(CGameState* pGameState, float fDelta) override;

    void OnSpellCastStart(CGameState* pGameState, CSpellCastStartEvent* pCastStartEvent);
private:
    NavigationMap* m_pMap;
};
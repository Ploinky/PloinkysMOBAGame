#pragma once

#include <GameState.h>
#include "system.h"

class NavigationMap;

class CNavigationSystem : public ISystem{
public:
    CNavigationSystem(NavigationMap* pMap);

    virtual void Update(CGameState* pGameState, float fDelta) override;

private:
    NavigationMap* m_pMap;
};
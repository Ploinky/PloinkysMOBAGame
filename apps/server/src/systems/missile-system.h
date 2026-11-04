#ifndef _MISSILE_SYSTEM_H_
#define _MISSILE_SYSTEM_H_

#include <common/game/game-system.h>

#include "game/server-game-state.h"
#include "events.h"

class CMissileSystem : public IGameSystem<CServerGameState> {
public:
    CMissileSystem();

    virtual void Update(CServerGameState* pGameState, float fDelta) override;
};

#endif
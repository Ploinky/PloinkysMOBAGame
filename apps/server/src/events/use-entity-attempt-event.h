#pragma once

#include "game/server-game-state.h"
#include "SpellTargetInfo.h"
#include "components/SpellCastComponent.h"

class CUseEntityAttemptEvent : public IGameEvent {
public:
    virtual std::type_index GetType() const override { return typeid(CUseEntityAttemptEvent); };

    UnitId idUser;
    UnitId idEntity;
    float x;
    float y;
};
#pragma once

#include "game/server-game-state.h"
#include "SpellTargetInfo.h"
#include "components/SpellCastComponent.h"

class CUnitSpawnEvent : public IGameEvent {
public:
    virtual std::type_index GetType() const override { return typeid(CUnitSpawnEvent); };

    std::string strTemplateId;
    Vector2 vec2Destination;
};
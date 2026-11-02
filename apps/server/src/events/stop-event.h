#pragma once

#include "../game/server-game-state.h"

class CStopEvent : public IGameEvent {
public:
    CStopEvent(UnitId idStop) : idStop(idStop) {};

    virtual std::type_index GetType() const override { return typeid(CStopEvent); };

    UnitId idStop;
};
#pragma once

class CGameState;

class ISystem {
public:
    virtual void Process(CGameState* pGameState, float fDelta) = 0;
};
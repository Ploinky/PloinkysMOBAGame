#pragma once

class CGameState;

class CWaveManager {
public:
    CWaveManager();

    void Update(CGameState* pGameState, float fDelta);

private:
    float fTimeSinceLastSpawn;
};
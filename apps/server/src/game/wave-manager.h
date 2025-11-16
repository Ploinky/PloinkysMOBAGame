#pragma once

class CServerGameState;

class CWaveManager {
public:
    CWaveManager();

    void Update(CServerGameState* pGameState, float fDelta);

private:
    float fTimeSinceLastSpawn;
};
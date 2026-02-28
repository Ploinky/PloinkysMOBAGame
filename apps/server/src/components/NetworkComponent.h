#pragma once

#include "common/pmg_types.h"

typedef struct NetworkComponent_t {
    public:
    NetworkComponent_t() : NetworkComponent_t(false) {};
    NetworkComponent_t(bool bSyncMovement) : m_bSyncMovement(bSyncMovement) {};
    bool SyncMovement() { return m_bSyncMovement; };
    void SetSyncMovement(bool bSyncMovement) { m_bSyncMovement = bSyncMovement; };
    bool IsSpawnSynced() { return m_bIsSpawnSynced; };
    void SetSpawnSynced() { m_bIsSpawnSynced = true; };
    UnitId idUnit;

private:
    bool m_bIsSpawnSynced = false;
    bool m_bSyncMovement = false;
} NetworkComponent_t;
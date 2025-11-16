#pragma once

#include "GameObject.h"

class CNetworkComponent : public IComponent {
public:
    CNetworkComponent(bool bSyncMovement) : m_bSyncMovement(bSyncMovement) {};
    bool SyncMovement() { return m_bSyncMovement; };
    void SetSyncMovement(bool bSyncMovement) { m_bSyncMovement = bSyncMovement; };
    bool IsSpawnSynced() { return m_bIsSpawnSynced; };
    void SetSpawnSynced() { m_bIsSpawnSynced = true; };

private:
    bool m_bIsSpawnSynced = false;
    bool m_bSyncMovement = false;
};
#pragma once

#include "common/pmg_types.h"

typedef struct NetworkSyncComponent_s {
    bool SyncSpawn;
    bool SyncDespawn;
    bool SyncTransform;
    bool SyncIdle;
    bool SyncStats;
    bool SyncAttackStart;
    bool SyncSpellCast;
    bool SyncCooldowns;
} NetworkSyncComponent_t;
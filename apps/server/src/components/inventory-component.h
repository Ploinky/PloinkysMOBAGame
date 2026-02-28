#pragma once

#include "common/PMG_Common.h"
#include "Components.h"
#include "common/pmg_types.h"

typedef struct InventoryComponent_t {
    UnitId idUnit;
    std::vector<UnitId> vecEntitiesInInventory;
} InventoryComponent_t;
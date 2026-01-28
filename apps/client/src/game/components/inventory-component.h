#pragma once

#include <vector>

#include "common/pmg_types.h"

typedef struct {
    UnitId idUnit;
	std::vector<UnitId> vecEntitiesInInventory;
} InventoryComponent_t;

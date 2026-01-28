#pragma once

#include "common/PMG_Common.h"
#include "Components.h"

class CInventoryComponent : public IComponent {
public:
    std::vector<UnitId> vecEntitiesInInventory;
};
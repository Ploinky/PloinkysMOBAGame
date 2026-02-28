#pragma once

#include "common/pmg_types.h"

typedef struct UseableComponent_t {
    int nUses = -1;
    CAbilityData abilityData;
} UseableComponent_t;
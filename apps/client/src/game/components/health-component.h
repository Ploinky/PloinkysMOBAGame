#pragma once
#include "common/pmg_types.h"

typedef struct {
	UnitId idUnit;
	int nHealth;
	int nMaxHealth;
	bool bIsDead;
} HealthComponent_t;

#pragma once

#include <vector>

class ParticleEffect;

typedef struct {
    UnitId idUnit;
    std::vector<ParticleEffect*> vecEffects;
} ParticleComponent_t;
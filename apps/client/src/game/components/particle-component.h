#pragma once

#include <vector>

class ParticleEffect;

typedef struct {
    std::vector<ParticleEffect*> vecEffects;
} ParticleComponent_t;
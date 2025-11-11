#pragma once

#include <vector>
#include <core/audio/audio-engine.h>

typedef struct {
    std::vector<HVoice> vecCurrentSounds;
    HEmitter hEmitter;
} AudioEmitterComponent_t;
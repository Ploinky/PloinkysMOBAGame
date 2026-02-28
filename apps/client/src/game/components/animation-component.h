#pragma once

#include <string>
#include "common/PMG_Common.h"

typedef struct {
    std::string m_strAnimationName;
    bool m_bLoop;
    float m_fAnimationTime;
    mat_t vecBones[256];
} AnimationComponent_t;
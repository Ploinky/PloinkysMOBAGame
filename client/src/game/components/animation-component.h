#pragma once

#include <string>

typedef struct {
    std::string m_strAnimationName;
    bool m_bLoop;
    float m_fAnimationTime;
    DirectX::XMFLOAT4X4 vecBones[256];
} AnimationComponent_t;
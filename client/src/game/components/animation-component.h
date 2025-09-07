#pragma once

#include <string>
#include <DirectXMath.h>

typedef struct {
    std::string m_strAnimationName;
    bool m_bLoop;
    float m_fAnimationTime;
    DirectX::XMFLOAT4X4 vecBones[256];
} AnimationComponent_t;
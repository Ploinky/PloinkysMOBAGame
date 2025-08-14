#pragma once

#include <DirectXMath.h>
#include "Model.h"

class ParticleSystem;

enum class ERenderCommandType {
    NONE,
    STATIC_MESH,
    SKINNED_MESH,
    PARTICLE_SYSTEM
};

typedef struct RenderCommand_t {
    ERenderCommandType eType = ERenderCommandType::NONE;

    DirectX::XMFLOAT4X4 worldMatrix;

    GameObject* pModel = nullptr;
    std::vector<DirectX::XMFLOAT4X4>* vecBones = nullptr;
    ParticleSystem* pSystem = nullptr;
} RenderCommand_t;
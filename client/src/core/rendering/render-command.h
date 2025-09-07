#pragma once

#include <DirectXMath.h>
#include "Model.h"
#include "core/graphics/graphics-engine.h"
#include "client-asset-manager.h"

class ParticleSystem;

enum class ERenderCommandType {
    NONE,
    STATIC_MESH,
    SKINNED_MESH,
    PARTICLE_SYSTEM
};

typedef struct RenderCommand_t {
    ERenderCommandType eType = ERenderCommandType::NONE;

    BufferHandle_t hVertexBuffer;
    BufferHandle_t hIndexBuffer;
    unsigned int uIndexCount;

    HTexture hTexture;

    DirectX::XMFLOAT4X4 worldMatrix;

    HModel hModel = INVALID_ASSET_HANDLE;
    std::string strAnimation = "";
    float fAnimTime = 0.0f;
    bool bDoLoop = false;
    Vector3 vec3Position;
    Vector3 vec3Rotation;
    DirectX::XMFLOAT4X4 vecBones[256];

    ParticleSystem* pSystem = nullptr;
} RenderCommand_t;
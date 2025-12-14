#pragma once

#include "Model.h"
#include "core/graphics/graphics-engine.h"
#include "client-asset-manager.h"

class ParticleEffect;

enum class ERenderCommandType {
    NONE,
    STATIC_MESH,
    SKINNED_MESH,
    PARTICLE_SYSTEM
};

typedef struct RenderCommand_t {
    ERenderCommandType eType = ERenderCommandType::NONE;

    HBuffer hVertexBuffer;
    HBuffer hIndexBuffer;
    HBuffer hInstanceBuffer;
    unsigned int uIndexCount;
    unsigned int uInstanceCount;

    HTexture hTexture;

    mat_t worldMatrix;

    HModel hModel = INVALID_ASSET_HANDLE;
    std::string strAnimation = "";
    float fAnimTime = 0.0f;
    bool bDoLoop = false;
    Vector3 vec3Position;
    Vector3 vec3Rotation;
    Vector3 vec3Scale;
    mat_t vecBones[256];

    const ParticleEffect* pSystem = nullptr;
} RenderCommand_t;
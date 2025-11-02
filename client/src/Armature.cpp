#include "Armature.h"
#include <fstream>
#include <common/PMG_Common.h>
#include <common/pmg_physics.h>

DirectX::XMMATRIX BonePosition::ToMatrix() const {
    return DirectX::XMMatrixRotationQuaternion(rotation) * DirectX::XMMatrixTranslation(translation.x, translation.y, translation.z);
};

BonePosition BonePosition::Interpolate(BonePosition from, BonePosition to, float pct) {
    assert(pct <= 1);
    assert(pct >= 0);

    BonePosition ret;

    ret.rotation = DirectX::XMQuaternionSlerp(from.rotation, to.rotation, pct);

    DirectX::XMVECTOR vec{
        std::lerp(DirectX::XMVectorGetX(from.rotation), DirectX::XMVectorGetX(to.rotation), pct),
        std::lerp(DirectX::XMVectorGetY(from.rotation), DirectX::XMVectorGetY(to.rotation), pct),
        std::lerp(DirectX::XMVectorGetZ(from.rotation), DirectX::XMVectorGetZ(to.rotation), pct),
        std::lerp(DirectX::XMVectorGetW(from.rotation), DirectX::XMVectorGetW(to.rotation), pct)
    };

    ret.rotation = vec;

    ret.translation = DirectX::XMFLOAT3(
            std::lerp(from.translation.x, to.translation.x, pct),
            std::lerp(from.translation.y, to.translation.y, pct),
            std::lerp(from.translation.z, to.translation.z, pct)
        );

    return ret;
}
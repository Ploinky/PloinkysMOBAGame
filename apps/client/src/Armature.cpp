#include "Armature.h"
#include <fstream>
#include <common/PMG_Common.h>
#include <common/pmg_physics.h>

mat BonePosition::ToMatrix() const {
    return mat::Rotation(rotation) * mat::Translation(translation.x, translation.y, translation.z);
};

BonePosition BonePosition::Interpolate(BonePosition from, BonePosition to, float pct) {
    BonePosition ret;

    ret.rotation = Slerp(from.rotation, to.rotation, pct);

    Quaternion vec{
        std::lerp(from.rotation.x, to.rotation.x, pct),
        std::lerp(from.rotation.y, to.rotation.y, pct),
        std::lerp(from.rotation.z, to.rotation.z, pct),
        std::lerp(from.rotation.w, to.rotation.w, pct)
    };

    ret.rotation = vec;

    ret.translation = Vector3(
            std::lerp(from.translation.x, to.translation.x, pct),
            std::lerp(from.translation.y, to.translation.y, pct),
            std::lerp(from.translation.z, to.translation.z, pct)
        );

    return ret;
}
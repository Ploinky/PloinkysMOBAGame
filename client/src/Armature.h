#pragma once

#include <string>
#include <vector>
#include <map>
#include <common/PMG_Common.h>

class BonePosition {
public:
	DirectX::XMVECTOR rotation;
	DirectX::XMFLOAT3 translation;
	DirectX::XMMATRIX ToMatrix() const;

	static BonePosition Interpolate(BonePosition from, BonePosition to, float pct);
};

class Bone {
public:
	int Index;
	int parent_index = -1;
};

class Armature {
public:
	std::vector<Bone> bones;
	std::vector<DirectX::XMMATRIX> global_inverse_bind_poses;
	std::vector<DirectX::XMMATRIX> currentPos;
	DirectX::XMMATRIX animation_palette[256];
};

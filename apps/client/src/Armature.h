#pragma once

#include <string>
#include <vector>
#include <map>
#include <common/PMG_Common.h>

class BonePosition {
public:
	Quaternion rotation;
	Vector3 translation;
	mat ToMatrix() const;

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
	std::vector<mat> global_inverse_bind_poses;
	std::vector<mat> currentPos;
	mat animation_palette[256];
};

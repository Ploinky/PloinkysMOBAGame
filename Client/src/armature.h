#pragma once

#include "pmg_physics.h"
#include <string>
#include <vector>

namespace PMG {
	class BonePosition {
	public:
		Physics::Quaternion rotation;
		Physics::Vector3 translation;
		Physics::mat_t ToMatrix() const;
	};

	class Bone {
	public:
		BonePosition bind_pose;
		std::string name;
		int parent_index;
	};

	class Armature {
	public:
		static Armature* LoadArmature(std::string file_name);
		void ComputeGlobalInverseBindPoses();

		std::vector<Bone> bones;
		std::vector<Physics::mat_t> global_inverse_bind_poses;

	};
}
#pragma once

#include <DirectXMath.h>
#include <string>
#include <vector>

namespace PMG {
	class BonePosition {
	public:
		DirectX::XMVECTOR rotation;
		DirectX::XMFLOAT3 translation;
		DirectX::XMMATRIX ToMatrix() const;
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
		std::vector<DirectX::XMMATRIX> global_inverse_bind_poses;

	};
}
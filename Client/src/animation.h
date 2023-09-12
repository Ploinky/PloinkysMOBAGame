#pragma once

#include <DirectXMath.h>
#include <string>
#include <vector>
#include "armature.h"

namespace PMG {
	class Animation {
	public:
		static Animation* LoadAnimation(std::string file_name);

		void GetGlobalPoseAtTime(std::vector<DirectX::XMMATRIX>& poses, const Armature* armature, double time);
		
		int bone_count;
		int frame_count;
		double duration;
		double frame_duration;
		std::vector<std::vector<BonePosition>> animation_tracks;
	};
}
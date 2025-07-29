#include "Animation.h"
#include <fstream>
#include <Common/PMG_Common.h>
#include "Armature.h"

std::map<int, BonePosition> Animation::GetBonePositions(float time, bool bLoop) {
	std::map<int, BonePosition> bonePositions;

	time /= 1000;

	while(time > duration) {
		if(bLoop) {
			time -= duration;
		} else {
			time = duration;
		}
	}

 	for (const auto& track : animation_tracks) {
		if(!bonePositions.contains(track.NodeIndex)) {
			BonePosition pos;
			bonePositions.emplace(track.NodeIndex, pos);
		}

		BonePosition& bp = bonePositions.at(track.NodeIndex);

		const auto& keyframes = track.Positions;

		if (keyframes.empty()) {
			continue;
		}

		// Find the right keyframes for interpolation
		const AnimationKeyFrame* prevKeyframe = nullptr;
		const AnimationKeyFrame* nextKeyframe = nullptr;

		for (size_t i = 0; i < keyframes.size(); ++i) {
			if (keyframes[i].Time >= time) {
				nextKeyframe = &keyframes[i];
				if (i > 0) {
					prevKeyframe = &keyframes[i - 1];
				}
				break;
			}
		}

		if(prevKeyframe == nullptr && nextKeyframe == nullptr) {
			nextKeyframe = &keyframes.back();
		}

		// if (!prevKeyframe) {
		// 	prevKeyframe = nextKeyframe;
		// }
// 
		// if (!nextKeyframe) {
		// 	nextKeyframe = prevKeyframe;
		// }

		BonePosition kfbp;
		if (prevKeyframe && nextKeyframe) {
			float factor = (time - prevKeyframe->Time) / (nextKeyframe->Time - prevKeyframe->Time);

			kfbp = InterpolateBonePosition(prevKeyframe->Position, nextKeyframe->Position, factor);
		} else if (prevKeyframe) {
			kfbp = prevKeyframe->Position;
		} else if (nextKeyframe) {
			kfbp = nextKeyframe->Position;
		}

		switch(track.Path) {
			case eGlbAnimationChannel_Translation:
				bp.translation = kfbp.translation;
				break;
			case eGlbAnimationChannel_Rotation:
				bp.rotation = kfbp.rotation;
				break;
			case eGlbAnimationChannel_Scale:
			default:
				break;
		}
	}

	return bonePositions;
}

BonePosition Animation::InterpolateBonePosition(const BonePosition& start, const BonePosition& end, float factor) {
	BonePosition result;
	result.translation = {
		start.translation.x + (end.translation.x - start.translation.x) * factor,
		start.translation.y + (end.translation.y - start.translation.y) * factor,
		start.translation.z + (end.translation.z - start.translation.z) * factor
	};
	result.rotation = DirectX::XMQuaternionSlerp(start.rotation, end.rotation, factor);
	return result;
}
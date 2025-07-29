#pragma once

#include <DirectXMath.h>
#include <string>
#include <vector>
#include <Common/PMG_Common.h>
#include <Armature.h>
#include <GLBFileLoader.h>

struct AnimationKeyFrame {
	BonePosition Position;
	float Time;
};

struct AnimationTrack {
	std::vector<AnimationKeyFrame> Positions;
	int NodeIndex;
	GLBANIMATIONCHANNEL Path;
};

class Animation {
public:
	std::map<int, BonePosition> GetBonePositions(float time, bool bLoop);
	BonePosition InterpolateBonePosition(const BonePosition& start, const BonePosition& end, float factor);

	int bone_count;
	int frame_count;
	float duration;
	float frame_duration;
	std::vector<AnimationTrack> animation_tracks;
};

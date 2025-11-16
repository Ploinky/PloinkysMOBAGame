#pragma once

#include <vector>
#include <string.h>
#include <string>
#include <map>
#include <stdint.h>
#include "Armature.h"
#include "common/PMG_Common.h"

// TODO enum class
enum GLBANIMATIONCHANNEL {
	eGlbAnimationChannel_None,
	eGlbAnimationChannel_Rotation,
	eGlbAnimationChannel_Translation,
	eGlbAnimationChannel_Scale,
};

struct UBYTE4 {
	uint8_t x;
	uint8_t y;
	uint8_t z;
	uint8_t w;
};

class BufferView {
public:
	uint32_t Buffer;
	uint32_t ByteOffset;
	uint32_t ByteLength;
	uint32_t ByteStride;
};

class Accessor {
public:
	uint32_t BufferView;
	uint32_t ByteOffset;
	uint32_t ComponentType;
	uint32_t Count;
	std::string type;
};

class GLBModelMaterial {
public:
	std::string TextureUri;
	std::vector<uint8_t> TextureData;
};

struct GLBVertexType {
	Vector3 Position;
	Vector3 Normals;
	Vector2 TextureCoordinates;
	UBYTE4 Joints;
	Vector4 Weights;
};

class GLBKeyFrame {
public:
	float Time;
	Vector3 Translation;
	Quaternion Rotation;
	Vector3 Scale;
};

class GLBAnimationChannel {
public:
	int TargetNode;
	std::vector<GLBKeyFrame> KeyFrames;
	GLBANIMATIONCHANNEL Path;
};

class GLBAnimation {
public:
	std::string Name;
	float Duration;
	std::vector<GLBAnimationChannel> Channels;
	
	std::map<int, BonePosition> GetBonePositions(float time, bool bLoop) {
		std::map<int, BonePosition> bonePositions;

		time /= 1000;

		while(time > Duration) {
			if(bLoop) {
				time -= Duration;
			} else {
				time = Duration;
			}
		}

		for (const auto& track : Channels) {
			if(!bonePositions.contains(track.TargetNode)) {
				BonePosition pos;
				bonePositions.emplace(track.TargetNode, pos);
			}

			BonePosition& bp = bonePositions.at(track.TargetNode);

			const auto& keyframes = track.KeyFrames;

			if (keyframes.empty()) {
				continue;
			}

			// Find the right keyframes for interpolation
			const GLBKeyFrame* prevKeyframe = nullptr;
			const GLBKeyFrame* nextKeyframe = nullptr;

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

			GLBKeyFrame kfbp;
			if (prevKeyframe && nextKeyframe) {
				float factor = (time - prevKeyframe->Time) / (nextKeyframe->Time - prevKeyframe->Time);

				kfbp = InterpolateBonePosition(prevKeyframe, nextKeyframe, factor);
			} else if (prevKeyframe) {
				kfbp = *prevKeyframe;
			} else if (nextKeyframe) {
				kfbp = *nextKeyframe;
			}

			switch(track.Path) {
				case eGlbAnimationChannel_Translation:
					bp.translation = kfbp.Translation;
					break;
				case eGlbAnimationChannel_Rotation:
					bp.rotation = kfbp.Rotation;
					break;
				case eGlbAnimationChannel_Scale:
				default:
					break;
			}
		}

		return bonePositions;
	}
	
	GLBKeyFrame InterpolateBonePosition(const GLBKeyFrame* start, const GLBKeyFrame* end, float factor) {
		GLBKeyFrame result;
		result.Translation = {
			start->Translation.x + (end->Translation.x - start->Translation.x) * factor,
			start->Translation.y + (end->Translation.y - start->Translation.y) * factor,
			start->Translation.z + (end->Translation.z - start->Translation.z) * factor
		};
		result.Rotation = Slerp(start->Rotation, end->Rotation, factor);
		return result;
	}
};

class GLBModelSkin {
public:
	std::string Name;
	std::vector<mat_t> InverseBindMatrices;
	std::vector<int> Joints;
};

class GLBModelMesh {
public:
	std::vector<GLBVertexType> Vertices;
	std::vector<uint32_t> Indices;
	int MaterialIndex = -1;
};

class GLBNode {
public:
	int NodeIndex;
	int Mesh;
	int Skin;
	Vector4 Rotation = Vector4(0, 0, 0, 1);
	Vector3 Scale = Vector3(0.0, 0.0, 0.0);
	Vector3 Translation = Vector3(0, 0, 0);
	std::string Name;
	std::vector<int> Children;
};

class GLBModel {
public:
	std::map<int, GLBNode> Nodes;
	std::map<int, GLBModelMesh> Meshes;
	std::map<int, GLBModelSkin> Skins;
	std::map<int, GLBModelMaterial> Materials;
	std::map<std::string, GLBAnimation> Animations;
};

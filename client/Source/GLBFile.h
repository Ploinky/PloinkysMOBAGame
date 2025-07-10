#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <string.h>
#include <string>
#include <map>
#include <stdint.h>

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
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Normals;
	DirectX::XMFLOAT2 TextureCoordinates;
	UBYTE4 Joints;
	DirectX::XMFLOAT4 Weights;
};

class GLBKeyFrame {
public:
	float Time;
	DirectX::XMFLOAT3 Translation;
	DirectX::XMFLOAT4 Rotation;
	DirectX::XMFLOAT3 Scale;
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
	std::vector<GLBAnimationChannel*> Channels;
};

class GLBModelSkin {
public:
	std::string Name;
	std::vector<DirectX::XMMATRIX> InverseBindMatrices;
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
	DirectX::XMFLOAT4 Rotation = DirectX::XMFLOAT4(0, 0, 0, 1);
	DirectX::XMFLOAT3 Scale = DirectX::XMFLOAT3(1, 1, 1);
	DirectX::XMFLOAT3 Translation = DirectX::XMFLOAT3(0, 0, 0);
	std::string Name;
	std::vector<int> Children;
};

class GLBModel {
public:
	std::map<int, GLBNode*> Nodes;
	std::map<int, GLBModelMesh*> Meshes;
	std::map<int, GLBModelSkin*> Skins;
	std::map<int, GLBModelMaterial*> Materials;
	std::map<std::string, GLBAnimation*> Animations;
};

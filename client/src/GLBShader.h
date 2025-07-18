#pragma once

#include <DirectXMath.h>
#include <Common/PMG_Common.h>
#include <Direct3D.h>

typedef struct {
	float position[3];
	float normal[3];
	float texCoord[2];
	uint8_t joints[4];
	float weights[4];
} glb_shader_vertex_t;

typedef struct {
	DirectX::XMFLOAT4X4 projMatrix;
	DirectX::XMFLOAT4X4 cameraMatrix;
} glb_shader_frame_const_t;

typedef struct {
	DirectX::XMFLOAT4X4 modelMatrix;
} glb_shader_model_const_t;

typedef struct {
	DirectX::XMFLOAT4X4 boneTransforms[256];
} glb_shader_mesh_const_t;

class GLBShader {
public:
	GLBShader();
	~GLBShader();
	void Initialize(Direct3D* direct3D, AssetManager* assetManager);

	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;

	ID3D11InputLayout* m_inputLayout;
	ID3D11SamplerState* samplerState_;
	ID3D11Buffer* m_frameConstBuffer;
	glb_shader_frame_const_t m_frameConstData;
	ID3D11Buffer* m_modelConstBuffer;
	glb_shader_model_const_t m_modelConstData;
	ID3D11Buffer* m_meshConstBuffer;
	glb_shader_mesh_const_t m_meshConstData;

};

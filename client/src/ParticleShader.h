#pragma once

#include <DirectXMath.h>
#include <Direct3D.h>
#include <Common/PMG_Common.h>

typedef struct {
	float position[3];
	float tex_coord[2];
} particle_shader_vertex_t;

typedef struct {
	float instance_position[3];
} particle_instance_data_t;

typedef struct {
	DirectX::XMFLOAT4X4 projMatrix;
	DirectX::XMFLOAT4X4 cameraMatrix;
	DirectX::XMFLOAT4X4 billboard_matrix;
} particle_shader_frame_const_t;

typedef struct {
	DirectX::XMFLOAT4X4 modelMatrix;
} particle_shader_model_const_t;

class ParticleShader {
public:
	ParticleShader() {};
	~ParticleShader();
	void Initialize(Direct3D* direct3D, AssetManager* assetManager);

	ID3D11Buffer* m_frameConstBuffer;
	particle_shader_frame_const_t m_frameConstData;
	ID3D11Buffer* m_modelConstBuffer;
	particle_shader_model_const_t m_modelConstData;
	ID3D11SamplerState* m_samplerState;
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_inputLayout;
};

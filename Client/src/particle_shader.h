#pragma once

#include <DirectXMath.h>
#include "shader.h"
#include "direct3d.h"

namespace PMG {
	typedef struct {
		float position[3];
		float color[4];
	} particle_shader_vertex_t;

	typedef struct {
		float instance_position[3];
	} particle_instance_data_t;

	typedef struct {
		DirectX::XMFLOAT4X4 projMatrix;
		DirectX::XMFLOAT4X4 cameraMatrix;
	} particle_shader_frame_const_t;

	typedef struct {
		DirectX::XMFLOAT4X4 modelMatrix;
	} particle_shader_model_const_t;

	class ParticleShader : public Shader {
	public:
		ParticleShader() : Shader(ShaderType::PARTICLE) {};
		~ParticleShader() {};
		void Initialize(Direct3D* direct3D);

		ID3D11Buffer* m_frameConstBuffer;
		particle_shader_frame_const_t m_frameConstData;
		ID3D11Buffer* m_modelConstBuffer;
		particle_shader_model_const_t m_modelConstData;
	};
}
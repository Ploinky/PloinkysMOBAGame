#pragma once

#include <d3d11.h>
#include <string>
#include <DirectXMath.h>
#include "pmg_physics.h"

namespace PMG {
	class Direct3D;

	enum ShaderType {
		COLOR,
		TEXTURE,
		NONE
	};

	typedef struct {
		float position[3];
		float color[4];
	} color_shader_vertex_t;

	class Shader {
	public:
		Shader(ShaderType type);
		virtual ~Shader();
		void Initialize(Direct3D* direct3D, const std::string& vsFile, const std::string& psFile, D3D11_INPUT_ELEMENT_DESC layoutDesc[], int inputElementCount);
		
		ID3D11VertexShader* m_vertexShader;
		ID3D11PixelShader* m_pixelShader;
		ID3D11InputLayout* m_inputLayout;
		ShaderType m_type;
	};

	typedef struct {
		Physics::mat_t projMatrix;
		DirectX::XMFLOAT4X4 cameraMatrix;
	} color_shader_frame_const_t;

	typedef struct {
		DirectX::XMFLOAT4X4 modelMatrix;
	} color_shader_model_const_t;

	class ColorShader : public Shader {
	public:
		ColorShader();
		~ColorShader();
		void Initialize(Direct3D* direct3D);

		ID3D11Buffer* m_frameConstBuffer;
		color_shader_frame_const_t m_frameConstData;
		ID3D11Buffer* m_modelConstBuffer;
		color_shader_model_const_t m_modelConstData;
	};

	typedef struct {
		float position[3];
		float texCoord[2];
	} texture_shader_vertex_t;

	typedef struct {
		Physics::mat_t projMatrix;
		DirectX::XMFLOAT4X4 cameraMatrix;
	} texture_shader_frame_const_t;

	typedef struct {
		DirectX::XMFLOAT4X4 modelMatrix;
	} texture_shader_model_const_t;

	class TextureShader : public Shader {
	public:
		TextureShader();
		~TextureShader();
		void Initialize(Direct3D* direct3D);

		ID3D11Buffer* m_frameConstBuffer;
		texture_shader_frame_const_t m_frameConstData;
		ID3D11Buffer* m_modelConstBuffer;
		texture_shader_model_const_t m_modelConstData;
		ID3D11SamplerState* m_samplerState;
	};
}
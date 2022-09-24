#include "shader.hpp"
#include "direct3D.hpp"
#include "util.hpp"

namespace P3D {
	Shader::Shader(ShaderType type) : m_type(type) {
		m_inputLayout = 0;
		m_pixelShader = 0;
		m_vertexShader = 0;
	}
	
	void Shader::Initialize(Direct3D* direct3D, const std::string& vsFile, const std::string& psFile, D3D11_INPUT_ELEMENT_DESC inputLayoutDescription[], int inputElementCount) {
		file_t shaderByteCode = Util::ReadBytesFromFile(vsFile);// "./shaders/vertex.cso");
		file_t psByteCode = Util::ReadBytesFromFile(psFile); // "./shaders/pixel.cso");

		HRESULT hr = direct3D->device->CreateVertexShader(shaderByteCode.data, shaderByteCode.size, nullptr, &m_vertexShader);

		if (FAILED(hr)) {
			printf("Failed to load color vertex shader!");
			return;
		}

		hr = direct3D->device->CreatePixelShader(psByteCode.data, psByteCode.size, nullptr, &m_pixelShader);

		if (FAILED(hr)) {
			printf("Failed to load color pixel shader!");
			return;
		}

		hr = direct3D->device->CreateInputLayout(inputLayoutDescription, inputElementCount, shaderByteCode.data, shaderByteCode.size, &m_inputLayout);

		if (FAILED(hr)) {
			printf("Failed to load color shader input layout!");
		}
	}

	ColorShader::ColorShader() : Shader(ShaderType::COLOR) {
	}

	void ColorShader::Initialize(Direct3D* direct3D) {
		D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[]{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		Shader::Initialize(direct3D, "./shaders/vertex_vs.cso", "./shaders/pixel_ps.cso", inputLayoutDesc, 2);

		D3D11_BUFFER_DESC desc;
		desc.ByteWidth = sizeof(m_frameConstData);
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = &m_frameConstData;

		HRESULT hr = direct3D->device->CreateBuffer(&desc, &data, &m_frameConstBuffer);

		if (FAILED(hr)) {
			printf("Failed to create color shader frame constant buffer!");
			return;
		}

		desc.ByteWidth = sizeof(m_modelConstData);
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		data.pSysMem = &m_modelConstData;

		hr = direct3D->device->CreateBuffer(&desc, &data, &m_modelConstBuffer);

		if (FAILED(hr)) {
			printf("Failed to create color shader model constant buffer!");
			return;
		}
	}

	TextureShader::TextureShader() : Shader(ShaderType::TEXTURE) {

	}

	void TextureShader::Initialize(Direct3D* direct3D) {
		D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[]{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		Shader::Initialize(direct3D, "./shaders/texture_vs.cso", "./shaders/texture_ps.cso", inputLayoutDesc, 2);

		D3D11_BUFFER_DESC desc;
		desc.ByteWidth = sizeof(m_frameConstData);
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = &m_frameConstData;

		HRESULT hr = direct3D->device->CreateBuffer(&desc, &data, &m_frameConstBuffer);

		if (FAILED(hr)) {
			printf("Failed to create texture shader frame constant buffer!");
			return;
		}

		desc.ByteWidth = sizeof(m_modelConstData);
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		data.pSysMem = &m_modelConstData;

		hr = direct3D->device->CreateBuffer(&desc, &data, &m_modelConstBuffer);

		if (FAILED(hr)) {
			printf("Failed to create texture shader model constant buffer!");
			return;
		}

		D3D11_SAMPLER_DESC samplerDesc;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0;
		samplerDesc.BorderColor[2] = 0;
		samplerDesc.BorderColor[3] = 0;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		hr = direct3D->device->CreateSamplerState(&samplerDesc, &m_samplerState);

		if (FAILED(hr)) {
			printf("Failed to create texture shader model sampler state!");
			return;
		}
	}
}
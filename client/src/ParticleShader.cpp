#include "ParticleShader.h"

ParticleShader::~ParticleShader() {
	if (m_frameConstBuffer) {
		m_frameConstBuffer->Release();
		m_frameConstBuffer = nullptr;
	}
		
	if (m_modelConstBuffer) {
		m_modelConstBuffer->Release();
		m_modelConstBuffer = nullptr;
	}

	if (m_samplerState) {
		m_samplerState->Release();
		m_samplerState = nullptr;
	}

	if (m_vertexShader != nullptr) {
		m_vertexShader->Release();
		m_vertexShader = nullptr;
	}

	if (m_pixelShader != nullptr) {
		m_pixelShader->Release();
		m_pixelShader = nullptr;
	}

	if (m_inputLayout != nullptr) {
		m_inputLayout->Release();
		m_inputLayout = nullptr;
	}
}
void ParticleShader::Initialize(Direct3D* direct3D, AssetManager* assetManager) {
	D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[]{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"POSITION", 1, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
	};

	std::vector<uint8_t> shaderByteCode = assetManager->LoadFile("Shaders/particle_vs.cso");
	std::vector<uint8_t> psByteCode = assetManager->LoadFile("Shaders/texture_ps.cso");

	HRESULT hr = direct3D->device->CreateVertexShader(shaderByteCode.data(), shaderByteCode.size(), nullptr, &m_vertexShader);

	if (FAILED(hr)) {
		printf("Failed to load particle vertex shader!");
		return;
	}

	hr = direct3D->device->CreatePixelShader(psByteCode.data(), psByteCode.size(), nullptr, &m_pixelShader);

	if (FAILED(hr)) {
		printf("Failed to load particle pixel shader!");
		return;
	}

	hr = direct3D->device->CreateInputLayout(inputLayoutDesc, 3, shaderByteCode.data(), shaderByteCode.size(), &m_inputLayout);

	if (FAILED(hr)) {
		printf("Failed to load particle shader input layout!");
	}

	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = sizeof(m_frameConstData);
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &m_frameConstData;

	hr = direct3D->device->CreateBuffer(&desc, &data, &m_frameConstBuffer);

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

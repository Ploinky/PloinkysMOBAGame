#include "particle_shader.h"

namespace PMG {
	void ParticleShader::Initialize(Direct3D* direct3D) {
		D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[]{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"POSITION", 1, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1}
		};

		Shader::Initialize(direct3D, "./shaders/particle_vs.cso", "./shaders/pixel_ps.cso", inputLayoutDesc, 3);

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
}
#include <FlatUnitShader.h>

CFlatUnlitShader::CFlatUnlitShader() {
	m_pFrameConstBuffer = nullptr;
	m_pModelConstBuffer = nullptr;
	m_pInputLayout = nullptr;
	m_pPixelShader = nullptr;
	m_pVertexShader = nullptr;
	m_frameConstData = {};
	m_modelConstData = {};
}

CFlatUnlitShader::~CFlatUnlitShader() {
	if (m_pFrameConstBuffer != nullptr) {
		m_pFrameConstBuffer->Release();
	}

	if (m_pModelConstBuffer != nullptr) {
		m_pModelConstBuffer->Release();
	}

	if (m_pInputLayout != nullptr) {
		m_pInputLayout->Release();
	}

	if (m_pPixelShader != nullptr) {
		m_pPixelShader->Release();
	}

	if (m_pVertexShader != nullptr) {
		m_pVertexShader->Release();
	}
}

void CFlatUnlitShader::Initialize(Direct3D* direct3D, AssetManager* assetManager) {
	D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[]{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	std::vector<uint8_t> vecVsByteCode = assetManager->LoadFile("Shaders\\flat_unlit_vs.cso");
	std::vector<uint8_t> vecPsByteCode = assetManager->LoadFile("Shaders\\flat_unlit_ps.cso");

	HRESULT hr = direct3D->device->CreateVertexShader(vecVsByteCode.data(), vecVsByteCode.size(), nullptr, &m_pVertexShader);

	if (FAILED(hr)) {
		throw std::exception("Failed to load flat unlit vertex shader!");
	}

	hr = direct3D->device->CreatePixelShader(vecPsByteCode.data(), vecPsByteCode.size(), nullptr, &m_pPixelShader);
		
	if (FAILED(hr)) {
		throw std::exception("Failed to load flat unlit pixel shader!");
	}

	hr = direct3D->device->CreateInputLayout(inputLayoutDesc, 2, vecVsByteCode.data(), vecVsByteCode.size(), &m_pInputLayout);

	if (FAILED(hr)) {
		throw std::exception("Failed to load flat unlit input layout!");
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

	hr = direct3D->device->CreateBuffer(&desc, &data, &m_pFrameConstBuffer);

	if (FAILED(hr)) {
		printf("Failed to create flat unlit shader frame constant buffer!");
		return;
	}
	desc.ByteWidth = sizeof(m_modelConstData);
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	data.pSysMem = &m_modelConstData;

	hr = direct3D->device->CreateBuffer(&desc, &data, &m_pModelConstBuffer);

	if (FAILED(hr)) {
		printf("Failed to create flat unlit shader model constant buffer!");
		return;
	}
}

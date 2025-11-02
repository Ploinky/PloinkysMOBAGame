#include "d3d11-graphics-engine.h"

#include <Common/PMG_Common.h>
#include <comdef.h>
#include <sstream>
#include <vector>
#include <type_traits>
#include <DirectXMath.h>
#include "Settings.h"
#include "client-asset-manager.h"

inline ID3D11Buffer* D3DBuffer(BufferHandle_t bfr) {
    return static_cast<ID3D11Buffer*>(bfr.ptr);
}


IGraphicsEngine* IGraphicsEngine::Create(HWindow hWindow, int nWidth, int nHeight) {
    CD3D11GraphicsEngine* pEngine = new CD3D11GraphicsEngine();
    pEngine->Initialize(hWindow, false); // TODO
    return pEngine;
}

CD3D11GraphicsEngine::~CD3D11GraphicsEngine() {
#ifdef _DEBUG
    ID3D11Debug* debug = 0;
    m_pDevice->QueryInterface(__uuidof(ID3D11Debug), (VOID**)(&debug));
#endif

    m_vecTextures.clear();
    m_vecBitmaps.clear();

    ID3D11RenderTargetView* nullViews[] = { nullptr };
    m_pContext->OMSetRenderTargets(1, nullViews, nullptr);

    m_pContext->ClearState();

    alpha_blend_state->Release();
    alpha_blend_disabled_state->Release();

    dWriteFactory->Release();
    renderTargetView->Release();
    renderTarget2D->Release();
    d2d_factory_->Release();
    format->Release();
    depthView->Release();
    swapChain->SetFullscreenState(false, nullptr);
    swapChain->Release();
        
    if (depthState != nullptr) {
        depthState->Release();
        depthState = nullptr;
    }

    if (rasterizerState_ != nullptr) {
        rasterizerState_->Release();
        rasterizerState_ = nullptr;
    }

    m_pContext->Flush();
    m_pContext->Release();
    m_pDevice->Release();

#ifdef _DEBUG
    debug->ReportLiveDeviceObjects(D3D11_RLDO_IGNORE_INTERNAL | D3D11_RLDO_DETAIL);
    debug->Release();
#endif
}

bool CD3D11GraphicsEngine::Initialize(HWND windowHandle, bool full_screen) {
    // Save the window handle
    this->windowHandle = windowHandle;

    // Create resources needed for rendering
    // Return immediately on error since the following initializations will probably fail

    // Create device and context
    if (!CreateDevice()) {
        return false;
    }

    // Create swap chain
    if (!CreateSwapChain(full_screen)) {
        return false;
    }

    // Create alpha blend state
    if (!CreateAlphaBlendState()) {
        return false;
    }

    // Create depth stencil state
    if (!CreateDepthStencilState()) {
        return false;
    }

	if (!CreateRasterizerState()) {
		return false;
	}

    //Create back buffer and render target view
    if (!CreateBackBuffer()) {
        return false;
    }

    // Create depth buffer and depth stencil view
    if (!CreateDepthBuffer()) {
        return false;
    }

    // Create Direct2D surface for 2D rendering
    if (!Create2DSurface()) {
        return false;
    }

    if (!InitializeDirectWrite()) {
        return false;
    }

    // Further setup of rendering resources

    // Bind views to output merger stage
    BindViews();

    // Set viewport
    SetViewport();

    InputLayoutD3D11_t skinnedMeshLayout;
    D3D11_INPUT_ELEMENT_DESC* inputLayoutDesc = new D3D11_INPUT_ELEMENT_DESC[5]{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"JOINTS", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
    skinnedMeshLayout.pDescs = inputLayoutDesc;
    skinnedMeshLayout.nCount = 5;
    m_vecInputElementDescs.emplace(EVertexFormat::SKINNED_MESH, skinnedMeshLayout);
    
    InputLayoutD3D11_t particleLayout;
    D3D11_INPUT_ELEMENT_DESC* particleInputLayoutDesc = new D3D11_INPUT_ELEMENT_DESC[3]{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"POSITION", 1, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
	};
    particleLayout.pDescs = particleInputLayoutDesc;
    particleLayout.nCount = 3;
    m_vecInputElementDescs.emplace(EVertexFormat::PARTICLE, particleLayout);
    
    InputLayoutD3D11_t flatUnlitLayout;
    D3D11_INPUT_ELEMENT_DESC* flatUnlitLayoutDesc = new D3D11_INPUT_ELEMENT_DESC[2]{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
    flatUnlitLayout.pDescs = flatUnlitLayoutDesc;
    flatUnlitLayout.nCount = 2;
    m_vecInputElementDescs.emplace(EVertexFormat::STATIC_MESH, flatUnlitLayout);

    // CD3D11GraphicsEngine successfully initialized and ready for use
    return true;
}

bool CD3D11GraphicsEngine::CreateDevice() {
    // Setup for device creation
    int createDeviceFlags =
#ifdef _DEBUG
        D3D11_CREATE_DEVICE_DEBUG |
#endif
        D3D11_CREATE_DEVICE_BGRA_SUPPORT;

    // Create device with feature level 11.0
    const D3D_FEATURE_LEVEL requestedLevel[]{ D3D_FEATURE_LEVEL_11_0 };

    // CD3D11GraphicsEngine will fill this with the supported feature level
    D3D_FEATURE_LEVEL featureLevel;

    // Create CD3D11GraphicsEngine device and context
    HRESULT hr = D3D11CreateDevice(
        0,
        D3D_DRIVER_TYPE_HARDWARE,
        0,
        createDeviceFlags,
        requestedLevel,
        1,
        D3D11_SDK_VERSION,
        &m_pDevice,
        &featureLevel,
        &m_pContext
    );

    // Check if device was created successfully
    if (FAILED(hr) || m_pDevice == 0) {
        std::wostringstream os;
        os << L"Failed to create CD3D11GraphicsEngine device. " << _com_error(hr).ErrorMessage();
        Logger::WErr(os.str());
        return false;
    }

    // Check if our feature level is supported
    if (featureLevel != D3D_FEATURE_LEVEL_11_0) {
        Logger::Err("CD3D11GraphicsEngine feature level 11.0 not supported.");
        return false;
    }

    // Create a Direct2D render target that can draw into the surface in the swap chain
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2d_factory_);

    if (FAILED(hr) || d2d_factory_ == nullptr) {
        Logger::Err("Failed to created D2D1 factory");
        return false;
    }

    // CD3D11GraphicsEngine initialization was successful
    return true;
}

bool CD3D11GraphicsEngine::CreateSwapChain(bool full_screen) {
    bool isFullscreenExclusive = full_screen;
    // Swap chain description
    DXGI_SWAP_CHAIN_DESC1 sc = { 0 };
    sc.Width = 0; // Width from HWND
    sc.Height = 0; // Height from HWND
    sc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sc.Stereo = false;
    sc.SampleDesc.Count = 1;
    sc.SampleDesc.Quality = 0;
    sc.BufferCount = 2;
    sc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sc.Scaling = DXGI_SCALING_NONE; // Maybe go back to stretch?
    sc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    sc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
    sc.Flags = DXGI_PRESENT_DO_NOT_SEQUENCE | DXGI_MWA_NO_ALT_ENTER; // Remove this when properly double buffering

    DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsc = { 0 };
    fsc.Windowed = !isFullscreenExclusive;
    fsc.RefreshRate.Numerator = 144;
    fsc.RefreshRate.Denominator = 1;
    fsc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    fsc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    // Get IDXGIDevice from CD3D11GraphicsEngine device
    IDXGIDevice1* dxgiDevice = 0;

    HRESULT hr = m_pDevice->QueryInterface(__uuidof(IDXGIDevice1), (void**)&dxgiDevice);

    if (FAILED(hr) || dxgiDevice == 0) {
        std::wostringstream os;
        os << L"Could not query IDXGIDevice. " << _com_error(hr).ErrorMessage();
        Logger::WErr(os.str());
        return false;
    }
        
    dxgiDevice->SetMaximumFrameLatency(10);


    // Get IDXGIAdapter from IDXGIDevice
    IDXGIAdapter* dxgiAdapter = 0;

    hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter);

    if (FAILED(hr) || dxgiAdapter == 0) {
        std::wostringstream os;
        os << L"Could not get IDXGIAdapter from IDXGIDevice. " << _com_error(hr).ErrorMessage();
        Logger::WErr(os.str());
        return false;
    }

    // Get IDXGIFactory from IDXGIAdapter
    IDXGIFactory2* dxgiFactory = 0;

    hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), (void**)&dxgiFactory);

    if (FAILED(hr) || dxgiFactory == 0) {
        std::wostringstream os;
        os << L"Could not get IDXGIFactory from IDXGIAdapter. " << _com_error(hr).ErrorMessage();
        Logger::WErr(os.str());
        return false;
    }

    // Finally create IDXGISwapChain from description
    hr = dxgiFactory->CreateSwapChainForHwnd(m_pDevice, windowHandle, &sc, &fsc, nullptr, &swapChain);


    if (FAILED(hr) || swapChain == 0) {
        std::wostringstream os;
        os << L"Could not create IDXGISwapChain. " << _com_error(hr).ErrorMessage();
        Logger::WErr(os.str());
        return false;
    }

    // No alt enter fullscreen modes allowed
    hr = dxgiFactory->MakeWindowAssociation(windowHandle, DXGI_MWA_NO_ALT_ENTER);

	swapChain->SetFullscreenState(isFullscreenExclusive, nullptr);

    if (FAILED(hr)) {
        Logger::Err("Failed to prevent alt enter fullscreen mode");
        return false;
    }

    // Release resources used in swap chain creation that we no longer need
    dxgiDevice->Release();
    dxgiAdapter->Release();
    dxgiFactory->Release();

    // Initialization of swap chain successful
    return true;
}

bool CD3D11GraphicsEngine::CreateBackBuffer() {
    ID3D11Texture2D* backBuffer;

    // Get first buffer for back buffer
    HRESULT hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);

    if (FAILED(hr) || backBuffer == 0) {
        std::wostringstream os;
        os << L"Could not create back buffer. " << _com_error(hr).ErrorMessage();
        Logger::WErr(os.str());
        return false;
    }

    // Create CD3D11GraphicsEngine render target view
    hr = m_pDevice->CreateRenderTargetView(backBuffer, 0, &renderTargetView);

    if (FAILED(hr) || renderTargetView == 0) {
        std::wostringstream os;
        os << L"Could not create render target view. " << _com_error(hr).ErrorMessage();
        Logger::WErr(os.str());
        return false;
    }

    // Release pointer to back buffer after we're done with it
    backBuffer->Release();

    // Initialization of back buffer successful
    return true;
}

bool CD3D11GraphicsEngine::CreateDepthBuffer() {
    // Get window dimensions from win32 API to correctly set swap chain size
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
    HRESULT hr = swapChain->GetDesc1(&swapChainDesc);

    if (FAILED(hr)) {
        Logger::WErr(L"Failed to get window dimension when trying to create depth buffer.");
        return false;
    }

    D3D11_TEXTURE2D_DESC ds;
    ds.Width = swapChainDesc.Width;
    ds.Height = swapChainDesc.Height;
    ds.MipLevels = 1;
    ds.ArraySize = 1;
    ds.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    ds.SampleDesc.Count = 1;
    ds.SampleDesc.Quality = 0;
    ds.Usage = D3D11_USAGE_DEFAULT;
    ds.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    ds.CPUAccessFlags = 0;
    ds.MiscFlags = 0;


    // The depth buffer 'remembers' the depth value of each fragment
    // Each new fragment has its position tested against the existing fragment at that screen coordinate
    // If the new fragment is closer to the camera, it will be rendered
    // The depth buffer will be updated at that fragment to reflect the depth of the newly rendered fragment
    ID3D11Texture2D* depthBuffer;

    hr = m_pDevice->CreateTexture2D(&ds, 0, &depthBuffer);

    if (FAILED(hr) || depthBuffer == 0) {
        std::wostringstream os;
        os << L"Could not create depth buffer. " << _com_error(hr).ErrorMessage();
        Logger::WErr(os.str());
        return false;
    }

    // The depth view provides a view for the depth buffer
    // The buffer simply represents the depth data in memory
    hr = m_pDevice->CreateDepthStencilView(depthBuffer, 0, &depthView);

    if (FAILED(hr) || depthView == 0) {
        std::wostringstream os;
        os << L"Could not create depth view. " << _com_error(hr).ErrorMessage();
        Logger::WErr(os.str());
        return false;
    }

    depthBuffer->Release();

    // Depth buffer and stencil view successfully created
    return true;
}

bool CD3D11GraphicsEngine::Create2DSurface() {
    D2D1_RENDER_TARGET_PROPERTIES props = {
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
            0,
            0
    };
    IDXGISurface* surf;

    HRESULT hr = swapChain->GetBuffer(
        0,
        IID_PPV_ARGS(&surf)
    );

    if (FAILED(hr) || surf == 0) {
        return false;
    }

    hr = d2d_factory_->CreateDxgiSurfaceRenderTarget(
        surf,
        &props,
        &renderTarget2D);

    surf->Release();

    if (FAILED(hr)) {
        return false;
    }

    return true;
}

bool CD3D11GraphicsEngine::CreateAlphaBlendState() {
    D3D11_BLEND_DESC blend_desc{};
    blend_desc.IndependentBlendEnable = false;
    blend_desc.AlphaToCoverageEnable = false;
    blend_desc.RenderTarget[0].BlendEnable = TRUE;
    blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blend_desc.RenderTarget[0].RenderTargetWriteMask = 0x0f;//D3D11_COLOR_WRITE_ENABLE_ALL;

    alpha_blend_state = nullptr;
        
    HRESULT hr = m_pDevice->CreateBlendState(&blend_desc, &alpha_blend_state);

    if (FAILED(hr) || !alpha_blend_state) {
        Logger::Err("Failed to create alpha blend state");
        return false;
    }

    blend_desc.RenderTarget[0].BlendEnable = false;
    hr = m_pDevice->CreateBlendState(&blend_desc, &alpha_blend_disabled_state);

    if (FAILED(hr)) {
        Logger::Err("Failed to create alpha blend disabled state");
        return false;
    }

    float input[4] = { 0,0,0,0 };

    m_pContext->OMSetBlendState(alpha_blend_disabled_state, input, 1);

    return true;
}

bool CD3D11GraphicsEngine::CreateDepthStencilState() {
    D3D11_DEPTH_STENCIL_DESC depth_write_enabled_desc{};
    depth_write_enabled_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    depth_write_enabled_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    depth_write_enabled_desc.DepthEnable = true;


    HRESULT hr = m_pDevice->CreateDepthStencilState(&depth_write_enabled_desc, &depthState);
    return !FAILED(hr);
}

bool CD3D11GraphicsEngine::CreateRasterizerState() {
	D3D11_RASTERIZER_DESC rasterizerDesc{};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.FrontCounterClockwise = TRUE;
	rasterizerDesc.DepthClipEnable = TRUE;

	HRESULT hr = m_pDevice->CreateRasterizerState(&rasterizerDesc, &rasterizerState_);

	if(FAILED(hr)) {
		return false;
	}

	m_pContext->RSSetState(rasterizerState_);

	return true;
}

void CD3D11GraphicsEngine::EnableAlphaBlending() {
    float blendFactor[4] = { 0,0,0,0 };
    m_pContext->OMSetBlendState(alpha_blend_state, blendFactor, 0xffffffff);
}

void CD3D11GraphicsEngine::DisableAlphaBlending() {
    float blendFactor[4] = { 0,0,0,0 };
    m_pContext->OMSetBlendState(alpha_blend_disabled_state, blendFactor, 0xffffffff);
}

void CD3D11GraphicsEngine::EnableDepthStencilState() {
    m_pContext->OMSetDepthStencilState(depthState, 0);
}

void CD3D11GraphicsEngine::DisableDepthStencilState() {
    m_pContext->OMSetDepthStencilState(nullptr, 0);
}

bool CD3D11GraphicsEngine::InitializeDirectWrite() {
    HRESULT hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown**>(&dWriteFactory));

    hr = dWriteFactory->CreateTextFormat(
        L"Consolas",
        NULL,
        DWRITE_FONT_WEIGHT_REGULAR,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        16.0f,
        L"en-us",
        &format
    );

    if (FAILED(hr)) {
        return false;
    }

    hr = format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);

    if (FAILED(hr)) {
        return false;
    }

    hr = format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    if (FAILED(hr)) {
        return false;
    }

    return true;
}

void CD3D11GraphicsEngine::BindViews() {
    m_pContext->OMSetRenderTargets(1, &renderTargetView, depthView);

    // Where to set this?
    m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void CD3D11GraphicsEngine::SetViewport() {

    // Get window dimensions from win32 API to correctly set swap chain size
    RECT winRect;

    if (!GetClientRect(windowHandle, &winRect)) {
        Logger::WErr(L"Failed to get window dimension when trying to set viewport.");
        return;
    }

    D3D11_VIEWPORT vp;
    vp.TopLeftX = 0.0f;
    vp.TopLeftY = 0.0f;
    vp.Width = static_cast<float>(winRect.right - winRect.left);
    vp.Height = static_cast<float>(winRect.bottom - winRect.top);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;

    m_pContext->RSSetViewports(1, &vp);
}

void CD3D11GraphicsEngine::ClearScreen() {
    float color[]{ 0.0f, 0, 0, 1.0f };
    m_pContext->ClearRenderTargetView(renderTargetView, color);
    m_pContext->ClearDepthStencilView(depthView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    m_pContext->OMSetRenderTargets(1, &renderTargetView, depthView);
    renderTarget2D->BeginDraw();
}

bool CD3D11GraphicsEngine::Present() {
    HRESULT hr = renderTarget2D->EndDraw();

    if (FAILED(hr)) {
        Logger::Err("Failed to draw 2D content");
        return false;
    }

        hr = swapChain->Present(1, 0);

    if(FAILED(hr)) {
        std::wostringstream os;
        os << L"Error trying to present rendered image. <" << hr << ">: " << _com_error(hr).ErrorMessage();
        Logger::WErr(os.str());
        return false;
    }

    // Image was presented successfully
    return true;
}

BufferHandle_t CD3D11GraphicsEngine::CreateVertexBuffer(void* pVertices, size_t uSize, int nCount) {
    ID3D11Buffer* buffer;

    // Description used to create vertex buffer
    D3D11_BUFFER_DESC bd;
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = static_cast<UINT>(uSize);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA data;
    data.pSysMem = pVertices;
    data.SysMemPitch = 0;
    data.SysMemSlicePitch = 0;

    // Buffer created in GPU memory
    HRESULT hr = m_pDevice->CreateBuffer(&bd, &data, &buffer);

    if(FAILED(hr)) {
        Logger::FormatErr("Error creating D3D vertex Buffer: ", _com_error(hr).ErrorMessage());
        return BufferHandle_t {nullptr};
    } else {
        return BufferHandle_t {buffer};
    }
}

BufferHandle_t CD3D11GraphicsEngine::CreateIndexBuffer(uint32_t* pIndices, int nCount) {
    ID3D11Buffer* buffer;

    D3D11_BUFFER_DESC bufferDesc;
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = sizeof(uint32_t) * nCount;
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA data;
    data.pSysMem = pIndices;
    data.SysMemPitch = 0;
    data.SysMemSlicePitch = 0;

    HRESULT hr = m_pDevice->CreateBuffer(&bufferDesc, &data, &buffer);

    if(FAILED(hr)) {
        Logger::FormatErr("Error creating D3D index Buffer: ", _com_error(hr).ErrorMessage());
        return BufferHandle_t {nullptr};
    } else {
        return BufferHandle_t {buffer};
    }
}

BufferHandle_t CD3D11GraphicsEngine::CreateConstantBuffer(size_t uSize, void* pInitialData) {
    ID3D11Buffer* buffer;

	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = uSize;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = pInitialData;
    data.SysMemPitch = 0;
    data.SysMemSlicePitch = 0;

	HRESULT hr = m_pDevice->CreateBuffer(&desc, pInitialData != nullptr ? &data : nullptr, &buffer);

    if(FAILED(hr)) {
        Logger::FormatErr("Error creating D3D constant buffer: ", _com_error(hr).ErrorMessage());
        return BufferHandle_t {nullptr};
    } {
        return BufferHandle_t {buffer};
    }
}

void CD3D11GraphicsEngine::UpdateBuffer(BufferHandle_t hBuffer, const void* src, size_t size) {
    D3D11_MAPPED_SUBRESOURCE mappedResource = { 0 };
    if(FAILED(m_pContext->Map(D3DBuffer(hBuffer), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource))) {
        Logger::Err("Failed to map buffer for update");
        return;
    }
    memcpy(mappedResource.pData, src, size);
    m_pContext->Unmap(D3DBuffer(hBuffer), 0);
}

BufferHandle_t CD3D11GraphicsEngine::CreateInstanceBuffer(void* instances, int instance_count, size_t size) {
    ID3D11Buffer* buffer;

    D3D11_BUFFER_DESC bufferDesc;
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.ByteWidth = size * instance_count;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA data;
    data.pSysMem = instances;
    data.SysMemPitch = 0;
    data.SysMemSlicePitch = 0;

    HRESULT hr = m_pDevice->CreateBuffer(&bufferDesc, &data, &buffer);

    if (FAILED(hr)) {
        Logger::FormatErr("Error creating D3D instance Buffer: ",  _com_error(hr).ErrorMessage());
        return BufferHandle_t {nullptr};
    }
    else {
        return BufferHandle_t {buffer};
    }
}
    
void CD3D11GraphicsEngine::SetFullScreen(bool full_screen) {
    if (full_screen) {
        Logger::Msg("[DIRECT3D]: Going fullscreen!");
    }
    else {
        Logger::Msg("[DIRECT3D]: Going Windowed!");
    }

    // TODO error handling?
    if (FAILED(swapChain->SetFullscreenState(full_screen, NULL))) {
        MessageBox(NULL, TEXT("Failed to go full screen"), TEXT("Error"), MB_ICONERROR);
    }
}

void CD3D11GraphicsEngine::SetWindowDimensions(int width_, int height_) {
    m_pContext->OMSetRenderTargets(0, 0, 0);
    renderTargetView->Release();
    renderTarget2D->Release();
    depthView->Release();
    m_pContext->ClearState();
    m_pContext->Flush();

    HRESULT hr = swapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

    Logger::Msg(std::string("[DIRECT3D]: new dimension ")
        .append(std::to_string(width_))
        .append("-")
        .append(std::to_string(height_)));
        if (FAILED(hr)) {
        Logger::Err("Failed to resize swapchain buffers");
    }

    if (!CreateBackBuffer()) {
        Logger::Err("Failed to create back buffer");
    }

    if (!CreateDepthBuffer()) {
        Logger::Err("Failed to create depth buffer");
    }

    if (!Create2DSurface()) {
        Logger::Err("Failed to create D2D surface");
    }

    BindViews();

    SetViewport();
}

void CD3D11GraphicsEngine::BindVertexShaderConstantBuffer(int nSlot, BufferHandle_t hBuffer) {
    ID3D11Buffer* pRawBuf = D3DBuffer(hBuffer);
    m_pContext->VSSetConstantBuffers(nSlot, 1, &pRawBuf);
}

void CD3D11GraphicsEngine::BindShaderProgram(HShaderProgram hShaderProgram) {
    if(hShaderProgram == INVALID_HANDLE) {
        Logger::Err("Failed to draw: invalid shader program handle");
        return;
    }

    ShaderProgramD3D11_t shaderProgram = m_vecShaderPrograms[hShaderProgram];

    m_pContext->VSSetShader(shaderProgram.pVertexShader, nullptr, 0);
    m_pContext->PSSetShader(shaderProgram.pPixelShader, nullptr, 0);
    m_pContext->IASetInputLayout(shaderProgram.pLayout);

    if(shaderProgram.pSampler != nullptr) {
        m_pContext->PSSetSamplers(0, 1, &shaderProgram.pSampler);
    }
}

void CD3D11GraphicsEngine::BindSampler(uint32_t slot, HSampler hSampler) {
    ID3D11SamplerState* pState = m_vecSamplers[hSampler];
    m_pContext->PSSetSamplers(slot, 1, &pState);
}

HShaderProgram CD3D11GraphicsEngine::LoadShaderProgram(std::string strShaderName, EVertexFormat eVertexFormat, std::vector<uint8_t> vecVsBytecode, std::vector<uint8_t> vecPsBytecode) {
    ShaderProgramD3D11_t shaderProgram;

    m_pDevice->CreateVertexShader(vecVsBytecode.data(), vecVsBytecode.size(), nullptr, &shaderProgram.pVertexShader);
    m_pDevice->CreatePixelShader(vecPsBytecode.data(), vecPsBytecode.size(), nullptr, &shaderProgram.pPixelShader);

    InputLayoutD3D11_t layout = m_vecInputElementDescs.at(eVertexFormat);
    m_pDevice->CreateInputLayout(layout.pDescs, layout.nCount, vecVsBytecode.data(), vecVsBytecode.size(), &shaderProgram.pLayout);

    D3D11_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    m_pDevice->CreateSamplerState(&samplerDesc, &shaderProgram.pSampler);

    m_vecShaderPrograms.push_back(shaderProgram);
    return m_vecShaderPrograms.size() - 1;
}

HTexture CD3D11GraphicsEngine::LoadTexture(unsigned char* pImageData, int uWidth, int uHeight) {
    ID3D11Texture2D* texture = 0;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = pImageData;
    initData.SysMemPitch = uWidth * 4;

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = uWidth;
    desc.Height = uHeight;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    HRESULT hr = m_pDevice->CreateTexture2D(&desc, &initData, &texture);

    if (FAILED(hr) || texture == nullptr) {
        return;
    }

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pShaderResourceView = nullptr;
    hr = m_pDevice->CreateShaderResourceView(texture, nullptr, &pShaderResourceView);
    texture->Release();

    if(FAILED(hr)) {
        return INVALID_ASSET_HANDLE;
    }

    m_vecTextures.push_back(pShaderResourceView);
    return m_vecTextures.size() - 1;
}

void CD3D11GraphicsEngine::BindTexture(uint32_t uSlot, HTexture hTexture) {
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTex = m_vecTextures.at(hTexture);
    m_pContext->PSSetShaderResources(uSlot, 1, &pTex);
}

void CD3D11GraphicsEngine::SetVertexBuffer(uint32_t uSlot, BufferHandle_t& vertexBuffer, UINT uStride, UINT uOffset) {
    ID3D11Buffer* ptr = D3DBuffer(vertexBuffer);
    m_pContext->IASetVertexBuffers(uSlot, 1, &ptr, &uStride, &uOffset);
}

void CD3D11GraphicsEngine::SetIndexBuffer(BufferHandle_t& indexBuffer) {
    ID3D11Buffer* ptr = D3DBuffer(indexBuffer);
    m_pContext->IASetIndexBuffer(ptr, DXGI_FORMAT_R32_UINT, 0);
}

void CD3D11GraphicsEngine::DrawIndexed(UINT indices) {
    m_pContext->DrawIndexed(indices, 0, 0);
}

void CD3D11GraphicsEngine::DrawInstanced(UINT uVertexCountPerInstance, UINT uInstanceCount) {
    m_pContext->DrawInstanced(uVertexCountPerInstance, uInstanceCount, 0, 0);
}


ICanvas2D* CD3D11GraphicsEngine::GetCanvas2D() {
    return this;
}

void CD3D11GraphicsEngine::RenderText(int x, int y, int w, int h, float color[3], std::string strText) {
    //Set the Font Color
    D2D1_COLOR_F FontColor = D2D1::ColorF(color[0], color[1], color[2], 1.0f);

    ID2D1SolidColorBrush* brush;
    HRESULT hr = renderTarget2D->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::Blue, 0.0f),
        &brush
    );

    if (FAILED(hr) || brush == nullptr) {
        return;
    }

    format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

    IDWriteTextLayout* textLayout;

    std::wstring wstr;
    int convertResult = MultiByteToWideChar(CP_UTF8, 0, strText.c_str(), (int)strlen(strText.c_str()), NULL, 0);
    wstr.resize(convertResult);
    convertResult = MultiByteToWideChar(CP_UTF8, 0, strText.c_str(), (int)strlen(strText.c_str()), &wstr[0], (int)wstr.size());

    hr = dWriteFactory->CreateTextLayout(
        wstr.c_str(),
        wstr.length(),
        format,
        static_cast<float>(w),
        static_cast<float>(h),
        &textLayout
    );

    if (FAILED(hr) || textLayout == nullptr) {
        return;
    }

    //Set the brush color D2D will use to draw with
    brush->SetColor(FontColor);

    //Create the D2D Render Area
    D2D1_POINT_2F point = D2D1::Point2F(static_cast<float>(x), static_cast<float>(y));

    //Draw the Text
    renderTarget2D->DrawTextLayout(
        point,
        textLayout,
        brush
    );

    brush->Release();
    textLayout->Release();
}

void CD3D11GraphicsEngine::DrawRect(int x, int y, int w, int h, float color[3]) {
    D2D1_RECT_F rect{};
    rect.left = static_cast<float>(x);
    rect.top = static_cast<float>(y);
    rect.right = static_cast<float>(x + w);
    rect.bottom = static_cast<float>(y + h);

    //Set the Font Color
    D2D1_COLOR_F c = D2D1::ColorF(color[0], color[1], color[2]);


    ID2D1SolidColorBrush* brush;
    HRESULT hr = renderTarget2D->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::Black, 1.0f),
        &brush
    );


    if (FAILED(hr) || brush == 0) {
        Logger::Err("Failed to create brush for rect");
        return;
    }

    brush->SetColor(c);

    renderTarget2D->DrawRectangle(&rect, brush);

    brush->Release();
}

void CD3D11GraphicsEngine::DrawShape(Vector2* points, int pointCount, float color[3]) {
    //Set the Font Color
    D2D1_COLOR_F c = D2D1::ColorF(color[0], color[1], color[2]);

    ID2D1SolidColorBrush* brush;
    HRESULT hr = renderTarget2D->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::Black, 1.0f),
        &brush
    );


    if (FAILED(hr) || brush == 0) {
        Logger::Err("Failed to create brush for rect");
        return;
    }

    brush->SetColor(c);

    ID2D1PathGeometry* geometry;
    ID2D1GeometrySink* geometrySink = NULL;

    d2d_factory_->CreatePathGeometry(&geometry);
    // Write to the path geometry using the geometry sink.
    geometry->Open(&geometrySink);
    geometrySink->BeginFigure({ static_cast<float>(points[0].x), static_cast<float>(points[0].y) }, D2D1_FIGURE_BEGIN_HOLLOW);

    for (int i = 1; i < pointCount; i++) {
        geometrySink->AddLine({ static_cast<float>(points[i].x), static_cast<float>(points[i].y) });
    }

    geometrySink->EndFigure(D2D1_FIGURE_END_CLOSED);
    hr = geometrySink->Close();

    if (FAILED(hr)) {
        Logger::Err("Failed to render shape");
    }

    renderTarget2D->DrawGeometry(geometry, brush);

    geometrySink->Release();
    geometry->Release();
    brush->Release();
}

void CD3D11GraphicsEngine::FillShape(Vector2* points, int pointCount, float color[3]) {
    if (pointCount < 2) {
        Logger::Err("Failed to draw shape: cannot draw shape from 1 point only");
        return;
    }

    //Set the Font Color
    D2D1_COLOR_F c = D2D1::ColorF(color[0], color[1], color[2]);

    ID2D1SolidColorBrush* brush;
    HRESULT hr = renderTarget2D->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::Black, 1.0f),
        &brush
    );


    if (FAILED(hr) || brush == 0) {
        Logger::Err("Failed to create brush for rect");
        return;
    }

    brush->SetColor(c);

    ID2D1PathGeometry* geometry;
    ID2D1GeometrySink* geometrySink = NULL;

    d2d_factory_->CreatePathGeometry(&geometry);
    // Write to the path geometry using the geometry sink.
    geometry->Open(&geometrySink);
    geometrySink->BeginFigure({ static_cast<float>(points[0].x), static_cast<float>(points[0].y) }, D2D1_FIGURE_BEGIN_FILLED);

    for (int i = 1; i < pointCount; i++) {
        geometrySink->AddLine({ static_cast<float>(points[i].x), static_cast<float>(points[i].y) });
    }

    geometrySink->EndFigure(D2D1_FIGURE_END_CLOSED);
    hr = geometrySink->Close();

    if (FAILED(hr)) {
        Logger::Err("Failed to render shape");
    }

    renderTarget2D->FillGeometry(geometry, brush);

    geometrySink->Release();
    geometry->Release();
    brush->Release();
}

void CD3D11GraphicsEngine::FillRect(int x, int y, int w, int h, float color[3]) {
    D2D1_RECT_F rect{};
    rect.left = static_cast<float>(x);
    rect.top = static_cast<float>(y);
    rect.right = static_cast<float>(x + w);
    rect.bottom = static_cast<float>(y + h);

    //Set the Font Color
    D2D1_COLOR_F c = D2D1::ColorF(color[0], color[1], color[2], 1.0f);

    ID2D1SolidColorBrush* brush;
    HRESULT hr = renderTarget2D->CreateSolidColorBrush(c, &brush);


    if (FAILED(hr) || brush == 0) {
        Logger::Err("Failed to create brush for rect");
        return;
    }

    renderTarget2D->FillRectangle(&rect, brush);

    brush->Release();
}

void CD3D11GraphicsEngine::DrawImage(float x, float y, float w, float h, HBitmap hBmp) {
    Microsoft::WRL::ComPtr<ID2D1Bitmap> pBitmap = m_vecBitmaps[hBmp];
    renderTarget2D->DrawBitmap(pBitmap, D2D1::RectF(x, y, x + w, y + h));
}

HBitmap CD3D11GraphicsEngine::LoadBitmapImage(unsigned char* pImageData, int width, int height) {
    Microsoft::WRL::ComPtr<ID2D1Bitmap> pBitmap = nullptr;

    D2D1_BITMAP_PROPERTIES bitmapProps = {};
    bitmapProps.pixelFormat.format = DXGI_FORMAT_R8G8B8A8_UNORM;
    bitmapProps.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
    bitmapProps.dpiX = 96.0f;
    bitmapProps.dpiY = 96.0f;
    // bitmapProps.bitmapOptions = D2D1_BITMAP_OPTIONS_NONE;

    // pitch (bytes per row)
    UINT32 stride = width * 4; // 4 bytes per pixel for RGBA8

    HRESULT hr = renderTarget2D->CreateBitmap(
        D2D1_SIZE_U{ (UINT32)width, (UINT32)height },
        (void*) pImageData,      // unsigned char* from stbi
        (UINT32) stride,
        &bitmapProps,
        &pBitmap
    );

    if (FAILED(hr)) {
        return INVALID_ASSET_HANDLE;
    }

    m_vecBitmaps.push_back(pBitmap);

    return m_vecBitmaps.size() - 1;
}
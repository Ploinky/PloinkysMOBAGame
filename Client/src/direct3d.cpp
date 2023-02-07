#include "direct3d.h"
#include "mesh.h"
#include "logger.h"
#include "vertex.h"
#include <comdef.h>
#include <sstream>
#include <vector>
#include <type_traits>
#include "util.h"
#include <DirectXMath.h>
#include "settings.h"

namespace PMG {
    Direct3D::~Direct3D() {
#ifdef _DEBUG
        ID3D11Debug* debug = 0;
        device->QueryInterface(__uuidof(ID3D11Debug), (VOID**)(&debug));
#endif

        ID3D11RenderTargetView* nullViews[] = { nullptr };
        context->OMSetRenderTargets(1, nullViews, nullptr);

        context->ClearState();

        dWriteFactory->Release();
        renderTargetView->Release();
        renderTarget2D->Release();
        format->Release();
        depthView->Release();
        swapChain->SetFullscreenState(false, nullptr);
        swapChain->Release();

        context->Flush();
        context->Release();
        device->Release();

#ifdef _DEBUG
        debug->ReportLiveDeviceObjects(D3D11_RLDO_IGNORE_INTERNAL);
        debug->Release();
#endif
    }

    bool Direct3D::Initialize(HWND windowHandle) {
        // Save the window handle
        this->windowHandle = windowHandle;

        // Create resources needed for rendering
        // Return immediately on error since the following initializations will probably fail

        // Create device and context
        if (!CreateDevice()) {
            return false;
        }

        // Create swap chain
        if (!CreateSwapChain()) {
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

        // Direct3D successfully initialized and ready for use
        return true;
    }

    bool Direct3D::CreateDevice() {
        // Setup for device creation
        int createDeviceFlags =
#ifdef _DEBUG
            D3D11_CREATE_DEVICE_DEBUG |
#endif
            D3D11_CREATE_DEVICE_BGRA_SUPPORT;

        // Create device with feature level 11.0
        const D3D_FEATURE_LEVEL requestedLevel[]{ D3D_FEATURE_LEVEL_11_0 };

        // Direct3D will fill this with the supported feature level
        D3D_FEATURE_LEVEL featureLevel;

        // Create Direct3D device and context
        HRESULT hr = D3D11CreateDevice(
            0,
            D3D_DRIVER_TYPE_HARDWARE,
            0,
            createDeviceFlags,
            requestedLevel,
            1,
            D3D11_SDK_VERSION,
            &device,
            &featureLevel,
            &context
        );

        // Check if device was created successfully
        if (FAILED(hr) || device == 0) {
            std::wostringstream os;
            os << L"Failed to create Direct3D device. " << _com_error(hr).ErrorMessage();
            Logger::WErr(os.str());
            return false;
        }

        // Check if our feature level is supported
        if (featureLevel != D3D_FEATURE_LEVEL_11_0) {
            Logger::Err("Direct3D feature level 11.0 not supported.");
            return false;
        }

        // Direct3D initialization was successful
        return true;
    }

    bool Direct3D::CreateSwapChain() {
        bool isFullscreenExclusive = Settings::GetInt(PMGSettings::WINDOW_MODE) == (int)WindowMode::FULLSCREEN;
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
        fsc.RefreshRate.Numerator = 14;
        fsc.RefreshRate.Denominator = 1;
        fsc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        fsc.Scaling = DXGI_MODE_SCALING_CENTERED;

        // Get IDXGIDevice from Direct3D device
        IDXGIDevice* dxgiDevice = 0;

        HRESULT hr = device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);

        if (FAILED(hr) || dxgiDevice == 0) {
            std::wostringstream os;
            os << L"Could not query IDXGIDevice. " << _com_error(hr).ErrorMessage();
            Logger::WErr(os.str());
            return false;
        }


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
        hr = dxgiFactory->CreateSwapChainForHwnd(device, windowHandle, &sc, isFullscreenExclusive ? &fsc : NULL, nullptr, &swapChain);


        if (FAILED(hr) || swapChain == 0) {
            std::wostringstream os;
            os << L"Could not create IDXGISwapChain. " << _com_error(hr).ErrorMessage();
            Logger::WErr(os.str());
            return false;
        }

        // No alt enter fullscreen modes allowed
        hr = dxgiFactory->MakeWindowAssociation(windowHandle, DXGI_MWA_NO_ALT_ENTER);

        if (isFullscreenExclusive) {
            swapChain->SetFullscreenState(true, nullptr);
        }

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

    bool Direct3D::CreateBackBuffer() {
        ID3D11Texture2D* backBuffer;

        // Get first buffer for back buffer
        HRESULT hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);

        if (FAILED(hr) || backBuffer == 0) {
            std::wostringstream os;
            os << L"Could not create back buffer. " << _com_error(hr).ErrorMessage();
            Logger::WErr(os.str());
            return false;
        }

        // Create Direct3D render target view
        hr = device->CreateRenderTargetView(backBuffer, 0, &renderTargetView);

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

    bool Direct3D::CreateDepthBuffer() {
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

        hr = device->CreateTexture2D(&ds, 0, &depthBuffer);

        if (FAILED(hr) || depthBuffer == 0) {
            std::wostringstream os;
            os << L"Could not create depth buffer. " << _com_error(hr).ErrorMessage();
            Logger::WErr(os.str());
            return false;
        }

        // The depth view provides a view for the depth buffer
        // The buffer simply represents the depth data in memory
        hr = device->CreateDepthStencilView(depthBuffer, 0, &depthView);

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

    bool Direct3D::Create2DSurface() {
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

        // Create a Direct2D render target that can draw into the surface in the swap chain
        ID2D1Factory* d2dFactory;
        hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2dFactory);

        if (FAILED(hr) || d2dFactory == 0) {
            surf->Release();
            return false;
        }

        hr = d2dFactory->CreateDxgiSurfaceRenderTarget(
            surf,
            &props,
            &renderTarget2D);

        surf->Release();
        d2dFactory->Release();

        if (FAILED(hr)) {
            return false;
        }

        return true;
    }

    bool Direct3D::InitializeDirectWrite() {
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

    void Direct3D::BindViews() {
        context->OMSetRenderTargets(1, &renderTargetView, depthView);
    }

    void Direct3D::SetViewport() {

        // Get window dimensions from win32 API to correctly set swap chain size
        RECT winRect;

        if (!GetWindowRect(windowHandle, &winRect)) {
            Logger::WErr(L"Failed to get window dimension when trying to set viewport.");
            return;
        }

        D3D11_VIEWPORT vp;
        vp.TopLeftX = 0.0f;
        vp.TopLeftY = 0.0f;
        vp.Width = winRect.right - winRect.left;
        vp.Height = winRect.bottom - winRect.top;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;

        context->RSSetViewports(1, &vp);
    }

    void Direct3D::ClearScreen() {
        float color[]{ 0.0f, 0.0f, 0.0f, 1.0f };
        context->ClearRenderTargetView(renderTargetView, color);
        context->ClearDepthStencilView(depthView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
        context->OMSetRenderTargets(1, &renderTargetView, depthView);
        renderTarget2D->BeginDraw();
    }

    bool Direct3D::Present() {
        HRESULT hr = renderTarget2D->EndDraw();

        if (FAILED(hr)) {
            Logger::Err("Failed to draw 2D content");
            return false;
        }

        hr = swapChain->Present(0, 0);

        if(FAILED(hr)) {
            std::wostringstream os;
            os << L"Error trying to present rendered image. <" << hr << ">: " << _com_error(hr).ErrorMessage();
            Logger::WErr(os.str());
            return false;
        }

        // Image was presented successfully
        return true;
    }

    ID3D11Buffer* Direct3D::CreateVertexBuffer(void* vertices, int vertexCount, size_t size) {
        ID3D11Buffer* buffer;

        // Description used to create vertex buffer
        D3D11_BUFFER_DESC bd;
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = size;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = 0;
        bd.MiscFlags = 0;

        D3D11_SUBRESOURCE_DATA data;
        data.pSysMem = vertices;
        data.SysMemPitch = 0;
        data.SysMemSlicePitch = 0;

        // Buffer created in GPU memory
        HRESULT hr = device->CreateBuffer(&bd, &data, &buffer);

        if(FAILED(hr)) {
            std::wostringstream os;
            os << L"Error creating D3D vertex Buffer. <" << hr << ">: " << _com_error(hr).ErrorMessage();
            Logger::WErr(os.str());
            return nullptr;
        } else {
            return buffer;
        }
    }

    ID3D11Buffer* Direct3D::CreateIndexBuffer(unsigned int* indices, int indexCount) {
        ID3D11Buffer* buffer;

        D3D11_BUFFER_DESC bufferDesc;
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.ByteWidth = sizeof(unsigned int) * indexCount;
        bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bufferDesc.CPUAccessFlags = 0;
        bufferDesc.MiscFlags = 0;

        D3D11_SUBRESOURCE_DATA data;
        data.pSysMem = indices;
        data.SysMemPitch = 0;
        data.SysMemSlicePitch = 0;

        HRESULT hr = device->CreateBuffer(&bufferDesc, &data, &buffer);

        if(FAILED(hr)) {
            std::wostringstream os;
            os << L"Error creating D3D index Buffer. <" << hr << ">: " << _com_error(hr).ErrorMessage();
            Logger::WErr(os.str());
            return nullptr;
        } else {
            return buffer;
        }
    }
    
    void Direct3D::SetWindowDimensions(int width, int height) {
        context->OMSetRenderTargets(0, 0, 0);
        renderTargetView->Release();
        renderTarget2D->Release();
        depthView->Release();
        context->ClearState();
        context->Flush();

        HRESULT hr = swapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

        Logger::Msg(std::string("new dimension ")
            .append(std::to_string(width))
            .append("-")
            .append(std::to_string(height))
            .append("\r\n"));
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
}

#pragma once

#include "core/graphics/graphics-engine.h"

#include <d3d11_1.h>
#include <d2d1_1.h>
#include <string>
#include <dwrite.h>
#include "wincodec.h"
#include <wrl/client.h>
#include <vector>
#include "common/PMG_Common.h"

typedef struct ShaderProgramD3D11_t {
    ID3D11VertexShader* pVertexShader;
    ID3D11PixelShader* pPixelShader;
    ID3D11InputLayout* pLayout;
    ID3D11SamplerState* pSampler;
} ShaderProgramD3D11_t;

typedef struct InputLayoutD3D11_t {
    D3D11_INPUT_ELEMENT_DESC* pDescs;
    int nCount;
    UINT uStride;
    UINT uOffset;
} InputLayoutD3D11_t;

class CD3D11GraphicsEngine : public IGraphicsEngine, ICanvas2D {
    public:
        ~CD3D11GraphicsEngine();
            
        // Initialize all resources that the Direct3D API needs
        // After this is done, Direct3D will be ready to use
        // Returns false (and logs the error) if initialization fails
        // Engine should immediately in this case
        bool Initialize(HWND windowHandle, bool full_screen);
        
        // Sets the native window's dimensions
        // This will resize swap chain buffers and recreate render target and depth view
        // This MUST be called whenever the size of the HWND that Direct3D is displayed in changes
        virtual void SetWindowDimensions(int nWidth, int nHeight) override;

        // Clears the render target view to the clearColor and clears the depth stencil view
        // Must be called before each rendered frame
        virtual void ClearScreen() override;

        // Presents the rendered image on the swap chain
        virtual bool Present() override;

        virtual void SetFullScreen(bool full_screen) override;

        // Creates a vertex buffer
        virtual BufferHandle_t CreateVertexBuffer(void* pVertices, size_t uSize, int nCount) override;

        //Creates an index buffer
        virtual BufferHandle_t CreateIndexBuffer(uint32_t* pIndices, int nCount) override;

        // Creates a constant buffer
        virtual BufferHandle_t CreateConstantBuffer(size_t uSize, void* pInitialData) override;

        // Update the contents of a buffer
        virtual void UpdateBuffer(BufferHandle_t hBuffer, const void* src, size_t size) override;
        
        // Creates an instance buffer
        virtual BufferHandle_t CreateInstanceBuffer(void* instances, int instance_count, size_t size) override;

        virtual void BindVertexShaderConstantBuffer(int nSlot, BufferHandle_t hBuffer) override;
        
        virtual void EnableAlphaBlending() override;
        virtual void DisableAlphaBlending() override;

        virtual void EnableDepthStencilState() override;
        virtual void DisableDepthStencilState() override;

        virtual HShaderProgram LoadShaderProgram(std::string strShaderName, EVertexFormat eVertexFormat, std::vector<uint8_t> vecVsBytecode, std::vector<uint8_t> vecPsBytecode) override;

        virtual void BindShaderProgram(HShaderProgram hShaderProgram) override;
        virtual void BindTexture(uint32_t uSlot, HTexture hTexture) override;
        
        virtual void SetVertexBuffer(uint32_t uSlot, BufferHandle_t& vertexBuffer, UINT uStride, UINT uOffset) override;
        virtual void SetIndexBuffer(BufferHandle_t& indexBuffer) override;
        
        virtual void DrawIndexed(UINT indices) override;
        virtual void DrawInstanced(UINT uVertexCountPerInstance, UINT uInstanceCount) override;

        void BindSampler(uint32_t slot, HSampler sampler);
        
        virtual ICanvas2D* GetCanvas2D() override;

        virtual void RenderText(int x, int y, int w, int h, float color[3], std::string strText) override;
        virtual void DrawRect(int x, int y, int w, int h, float color[3]) override;
        virtual void DrawShape(Vector2* points, int pointCount, float color[3]) override;
        virtual void FillShape(Vector2* points, int pointCount, float color[3]) override;
        virtual void FillRect(int x, int y, int w, int h, float color[3]) override;
        virtual void DrawImage(float x, float y, float w, float h, HBitmap hBmp) override;
        
        virtual HBitmap LoadBitmapImage(unsigned char* pImageData, int uWidth, int uHeight) override;
        virtual HTexture LoadTexture(unsigned char* pImageData, int uWidth, int uHeight) override;

        ID3D11Device* m_pDevice;
        ID3D11DeviceContext* m_pContext;

        ID3D11RenderTargetView* renderTargetView;
        ID2D1RenderTarget* renderTarget2D;
        ID3D11DepthStencilState* depthState;
        ID3D11DepthStencilView* depthView;
        ID3D11BlendState* alpha_blend_state;
        ID3D11BlendState* alpha_blend_disabled_state;
		ID3D11RasterizerState* rasterizerState_;
        IDWriteTextFormat* format;
        IDWriteFactory* dWriteFactory;
        IDXGISwapChain1* swapChain;
        ID2D1Factory* d2d_factory_;
        HWND windowHandle;

    private:
        // Creates the Direct3D device and context
        bool CreateDevice();
        // Creates the DXGI swap chain and back buffer
        bool CreateSwapChain(bool full_screen);
        // Creates back buffer for rendering and ID3D11RenderTargetView
        bool CreateBackBuffer();
        // Creates depth and stencil buffers
        bool CreateDepthBuffer();
        // Creates 2D rendering surface
        bool Create2DSurface();
        // Creates alpha blend state
        bool CreateAlphaBlendState();
        // Creates depth stencil state
        bool CreateDepthStencilState();
		// Creates rasterizer state
		bool CreateRasterizerState();

        bool InitializeDirectWrite();
        // Bind render target and depth buffer view to output merger stage
        void BindViews();
        // Sets the viewport to take up the entirety of the window
        // Must be called every time the window is resized
        void SetViewport();

        std::vector<ShaderProgramD3D11_t> m_vecShaderPrograms;
        std::vector<ID3D11SamplerState*> m_vecSamplers;
        std::unordered_map<EVertexFormat, InputLayoutD3D11_t> m_vecInputElementDescs;
        std::vector<Microsoft::WRL::ComPtr<ID2D1Bitmap>> m_vecBitmaps;
        std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_vecTextures;
};

#pragma once

#include <d3d11.h>

// Provides access to Direct3D API
namespace P3D {
    // Forward declarations
    class Mesh;
    class Vertex;
    
    class Direct3D {
        public:
            ~Direct3D();
            
            // Initialize all resources that the Direct3D API needs
            // After this is done, Direct3D will be ready to use
            // Returns false (and logs the error) if initialization fails
            // Engine should immediately in this case
            bool Initialize(HWND windowHandle);
        
            // Sets the native window's dimensions
            // This will resize swap chain buffers and recreate render target and depth view
            // This MUST be called whenever the size of the HWND that Direct3D is displayed in changes
            void SetWindowDimensions(int width, int height);

            // Clears the render target view to the clearColor and clears the depth stencil view
            // Must be called before each rendered frame
            void ClearScreen();

            // Presents the rendered image on the swap chain
            bool Present();

            // Creates a vertex buffer
            ID3D11Buffer* CreateVertexBuffer(Vertex* vertices, int vertexCount);

            //Creates an index buffer
            ID3D11Buffer* CreateIndexBuffer(unsigned int* indices, int indexCount);
            
            // Renders the specified model to the screen
            void Render(Mesh* model);
            
            ID3D11Device* device;
            ID3D11DeviceContext* context;

        private:
            ID3D11RenderTargetView* renderTargetView;
            ID3D11DepthStencilView* depthView;
            IDXGISwapChain* swapChain;
            HWND windowHandle;

            // Creates the Direct3D device and context
            bool CreateDevice();
            // Creates the DXGI swap chain and back buffer
            bool CreateSwapChain();
            // Creates back buffer for rendering and ID3D11RenderTargetView
            bool CreateBackBuffer();
            // Creates depth and stencil buffers
            bool CreateDepthBuffer();
            // Bind render target and depth buffer view to output merger stage
            void BindViews();
            // Sets the viewport to take up the entirety of the window
            // Must be called every time the window is resized
            void SetViewport();
    };
}
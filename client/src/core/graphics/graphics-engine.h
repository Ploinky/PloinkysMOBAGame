#pragma once

#include <stdint.h>
#include <limits>
#include <string>
#include <DirectXMath.h>
#include <client-asset-manager.h>
#include "canvas-2d.h"

#ifdef WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    typedef HWND HWindow;
#else
    typedef void* HWindow;
#endif

typedef struct BufferHandle_t {
    void* ptr = nullptr;
    explicit operator bool() const { return ptr != nullptr; }
} BufferHandle_t;

typedef uint32_t GraphicsEngineHandle;
constexpr GraphicsEngineHandle INVALID_HANDLE = std::numeric_limits<uint32_t>::max();
typedef GraphicsEngineHandle HShaderProgram;
typedef GraphicsEngineHandle HSampler;

enum class EVertexFormat {
    STATIC_MESH,
    SKINNED_MESH,
    PARTICLE
};

typedef struct {
	DirectX::XMFLOAT4X4 projMatrix;
	DirectX::XMFLOAT4X4 cameraMatrix;
} FrameConstants_t;

typedef struct {
	DirectX::XMFLOAT4X4 modelMatrix;
} ModelConstants_t;

typedef struct {
	DirectX::XMFLOAT4X4 boneTransforms[256];
} SkinnedModelConstants_t;

typedef struct {
	DirectX::XMFLOAT4X4 billboardMatrix;
} BillboardFrameConstants_t;

class IGraphicsEngine {
public:
    static IGraphicsEngine* Create(HWindow hWindow, int nWidth, int nHeight);

    // == loading
    virtual HShaderProgram LoadShaderProgram(std::string strShaderName, EVertexFormat eVertexFormat, CClientAssetManager* pAssetManager) = 0;
    virtual void LoadTextureDataToGPU(TextureAsset_t& textureAsset) = 0;

    // == buffer management
    virtual BufferHandle_t CreateVertexBuffer(void* pVertices, size_t uSize, int nCount) = 0;
    virtual BufferHandle_t CreateIndexBuffer(uint32_t* pIndices, int nCount) = 0;
    virtual BufferHandle_t CreateConstantBuffer(size_t uSize, void* pInitialData) = 0;
    virtual BufferHandle_t CreateInstanceBuffer(void* instances, int instance_count, size_t size) = 0;
    virtual void UpdateBuffer(BufferHandle_t hBuffer, const void* pData, size_t uSize) = 0;
    
    // == window events?
	virtual void SetWindowDimensions(int nWidth, int nHeight) = 0;
	virtual void SetFullScreen(bool bFullscreen) = 0;

    // == drawing
    virtual void ClearScreen() = 0;

    virtual void BindVertexShaderConstantBuffer(int nSlot, BufferHandle_t hBuffer) = 0;
    
    virtual void EnableAlphaBlending() = 0;
    virtual void DisableAlphaBlending() = 0;
    
    virtual void EnableDepthStencilState() = 0;
    virtual void DisableDepthStencilState() = 0;

    
    virtual void BindShaderProgram(HShaderProgram hProgram) = 0;
    virtual void BindSampler(uint32_t uSlot, HSampler hSampler) = 0;
    virtual void BindTexture(uint32_t uSlot, TextureAsset_t& textureAsset) = 0;

    virtual void SetVertexBuffer(uint32_t uSlot, BufferHandle_t& vertexBuffer, UINT uStride, UINT uOffset) = 0;
    virtual void SetIndexBuffer(BufferHandle_t& indexBuffer) = 0;

    virtual void DrawIndexed(UINT indices) = 0;
    virtual void DrawInstanced(UINT uVertexCountPerInstance, UINT uInstanceCount) = 0;

    virtual bool Present() = 0;

    // == 2d
    virtual ICanvas2D* GetCanvas2D() = 0;
};
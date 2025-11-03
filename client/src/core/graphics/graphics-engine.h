#pragma once

#include <stdint.h>
#include <string>
#include <common/PMG_Common.h>

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
	mat projMatrix;
	mat cameraMatrix;
} FrameConstants_t;

typedef struct {
	mat modelMatrix;
} ModelConstants_t;

typedef struct {
	mat boneTransforms[256];
} SkinnedModelConstants_t;

typedef struct {
	mat billboardMatrix;
} BillboardFrameConstants_t;

class CCLientAssetManager;

typedef ASSET_HANDLE HBitmap;
typedef ASSET_HANDLE HTexture;
typedef ASSET_HANDLE HModel;

class ICanvas2D {
public:
    virtual ~ICanvas2D() = default;

    virtual void RenderText(int x, int y, int w, int h, float color[3], std::string strText) = 0;
    virtual void DrawRect(int x, int y, int w, int h, float color[3]) = 0;
    virtual void DrawShape(Vector2* points, int pointCount, float color[3]) = 0;
    virtual void FillShape(Vector2* points, int pointCount, float color[3]) = 0;
    virtual void FillRect(int x, int y, int w, int h, float color[3]) = 0;
    virtual void DrawImage(float x, float y, float w, float h, HBitmap hBmp) = 0;
};

class IGraphicsEngine {
public:
    static IGraphicsEngine* Create(HWindow hWindow, int nWidth, int nHeight);

    // == loading
    virtual HShaderProgram LoadShaderProgram(std::string strShaderName, EVertexFormat eVertexFormat, std::vector<uint8_t> vecVsBytecode, std::vector<uint8_t> vecPsBytecode) = 0;
    virtual HBitmap LoadBitmapImage(unsigned char* pImageData, int uWidth, int uHeight) = 0;
    virtual HTexture LoadTexture(unsigned char* pImageData, int uWidth, int uHeight) = 0;

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
    virtual void BindTexture(uint32_t uSlot, HTexture hTexture) = 0;

    virtual void SetVertexBuffer(uint32_t uSlot, BufferHandle_t& vertexBuffer, UINT uStride, UINT uOffset) = 0;
    virtual void SetIndexBuffer(BufferHandle_t& indexBuffer) = 0;

    virtual void DrawIndexed(UINT indices) = 0;
    virtual void DrawInstanced(UINT uVertexCountPerInstance, UINT uInstanceCount) = 0;

    virtual bool Present() = 0;

    // == 2d
    virtual ICanvas2D* GetCanvas2D() = 0;
};
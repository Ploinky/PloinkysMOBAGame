#include <core/graphics/graphics-engine.h>

class COpenGLGraphicsEngine : public IGraphicsEngine {
public:

    HShaderProgram LoadShaderProgram(std::string strShaderName, EVertexFormat eVertexFormat, std::vector<uint8_t> vecVsBytecode, std::vector<uint8_t> vecPsBytecode) override;
    HBitmap LoadBitmapImage(unsigned char* pImageData, int uWidth, int uHeight) override;
    HTexture LoadTexture(unsigned char* pImageData, int uWidth, int uHeight) override;

    // == buffer management
    BufferHandle_t CreateVertexBuffer(void* pVertices, size_t uSize, int nCount) override;
    BufferHandle_t CreateIndexBuffer(uint32_t* pIndices, int nCount) override;
    BufferHandle_t CreateConstantBuffer(size_t uSize, void* pInitialData) override;
    BufferHandle_t CreateInstanceBuffer(void* instances, int instance_count, size_t size) override;
    void UpdateBuffer(BufferHandle_t hBuffer, const void* pData, size_t uSize) override;
    
    // == window events?
	void SetWindowDimensions(int nWidth, int nHeight) override;
	void SetFullScreen(bool bFullscreen) override;

    // == drawing
    void ClearScreen() override;

    void BindVertexShaderConstantBuffer(int nSlot, BufferHandle_t hBuffer) override;
    
    void EnableAlphaBlending() override;
    void DisableAlphaBlending() override;
    
    void EnableDepthStencilState() override;
    void DisableDepthStencilState() override;

    
    void BindShaderProgram(HShaderProgram hProgram) override;
    void BindSampler(uint32_t uSlot, HSampler hSampler) override;
    void BindTexture(uint32_t uSlot, HTexture hTexture) override;

    void SetVertexBuffer(uint32_t uSlot, BufferHandle_t& vertexBuffer, unsigned int uStride, unsigned int uOffset) override;
    void SetIndexBuffer(BufferHandle_t& indexBuffer) override;

    void DrawIndexed(unsigned int indices) override;
    void DrawInstanced(unsigned int uVertexCountPerInstance, unsigned int uInstanceCount) override;

    bool Present() override;

    // == 2d
    ICanvas2D* GetCanvas2D() override;
};
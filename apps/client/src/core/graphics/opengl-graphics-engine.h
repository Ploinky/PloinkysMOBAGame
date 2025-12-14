#include <core/graphics/graphics-engine.h>

class COpenGLGraphicsEngine : public IGraphicsEngine, ICanvas2D {
public:
    COpenGLGraphicsEngine(HWindow hWindow);
    virtual HShaderProgram LoadShaderProgram(std::string strShaderName, EVertexFormat eVertexFormat, std::vector<uint8_t> vecVsBytecode, std::vector<uint8_t> vecPsBytecode) override;
    virtual HBitmap LoadBitmapImage(unsigned char* pImageData, int uWidth, int uHeight) override;
    virtual HTexture LoadTexture(unsigned char* pImageData, int uWidth, int uHeight) override;

    // == buffer management
    virtual HBuffer CreateVertexBuffer(void* pVertices, size_t uSize, int nCount) override;
    virtual HBuffer CreateIndexBuffer(uint32_t* pIndices, int nCount) override;
    virtual HBuffer CreateConstantBuffer(size_t uSize, void* pInitialData) override;
    virtual HBuffer CreateInstanceBuffer(void* instances, int instance_count, size_t size) override;
    virtual void UpdateBuffer(HBuffer hBuffer, const void* pData, size_t uSize) override;
    
    // == window events?
	virtual void SetWindowDimensions(int nWidth, int nHeight) override;
	virtual void SetFullScreen(bool bFullscreen) override;

    // == drawing
    virtual void ClearScreen() override;

    virtual void BindVertexShaderConstantBuffer(int nSlot, HBuffer hBuffer) override;
    
    virtual void EnableAlphaBlending() override;
    virtual void DisableAlphaBlending() override;
    
    virtual void EnableDepthStencilState() override;
    virtual void DisableDepthStencilState() override;

    
    virtual void BindShaderProgram(HShaderProgram hProgram) override;
    virtual void BindSampler(uint32_t uSlot, HSampler hSampler) override;
    virtual void BindTexture(uint32_t uSlot, HTexture hTexture) override;

    virtual void SetVertexBuffer(uint32_t uSlot, HBuffer vertexBuffer, unsigned int uStride, unsigned int uOffset) override;
    virtual void SetIndexBuffer(HBuffer indexBuffer) override;

    virtual void DrawIndexed(unsigned int indices) override;
    virtual void DrawInstanced(unsigned int uVertexCountPerInstance, unsigned int uInstanceCount) override;

    virtual bool Present() override;

    // == 2d
    virtual ICanvas2D* GetCanvas2D() override;

    virtual void RenderText(int x, int y, int w, int h, float color[3], std::string strText) override;
    virtual void DrawRect(int x, int y, int w, int h, float color[3]) override;
    virtual void DrawShape(Vector2* points, int pointCount, float color[3]) override;
    virtual void FillShape(Vector2* points, int pointCount, float color[3]) override;
    virtual void FillRect(int x, int y, int w, int h, float color[3]) override;
    virtual void DrawImage(float x, float y, float w, float h, HBitmap hBmp) override;

private:
    HWindow m_hWindow;
};
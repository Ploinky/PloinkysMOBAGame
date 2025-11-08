#include "opengl-graphics-engine.h"
#include <GL/glut.h>

IGraphicsEngine* IGraphicsEngine::Create(HWindow hWindow, int nWidth, int nHeight) {
    return new COpenGLGraphicsEngine(hWindow);
}

COpenGLGraphicsEngine::COpenGLGraphicsEngine(HWindow hWindow) {
    m_hWindow = hWindow;
}

HShaderProgram COpenGLGraphicsEngine::LoadShaderProgram(std::string strShaderName, EVertexFormat eVertexFormat, std::vector<uint8_t> vecVsBytecode, std::vector<uint8_t> vecPsBytecode) {
    return INVALID_HANDLE;
}
HBitmap COpenGLGraphicsEngine::LoadBitmapImage(unsigned char* pImageData, int uWidth, int uHeight) {
    return INVALID_ASSET_HANDLE;
}
HTexture COpenGLGraphicsEngine::LoadTexture(unsigned char* pImageData, int uWidth, int uHeight) {
    return INVALID_ASSET_HANDLE;
}

// == buffer management
BufferHandle_t COpenGLGraphicsEngine::CreateVertexBuffer(void* pVertices, size_t uSize, int nCount) {
    return BufferHandle_t {.ptr = nullptr};
}
BufferHandle_t COpenGLGraphicsEngine::CreateIndexBuffer(uint32_t* pIndices, int nCount) {
    return BufferHandle_t {.ptr = nullptr};
}
BufferHandle_t COpenGLGraphicsEngine::CreateConstantBuffer(size_t uSize, void* pInitialData) {
    return BufferHandle_t {.ptr = nullptr};
}
BufferHandle_t COpenGLGraphicsEngine::CreateInstanceBuffer(void* instances, int instance_count, size_t size) {
    return BufferHandle_t {.ptr = nullptr};
}
void COpenGLGraphicsEngine::UpdateBuffer(BufferHandle_t hBuffer, const void* pData, size_t uSize) {

}

// == window events?
void COpenGLGraphicsEngine::SetWindowDimensions(int nWidth, int nHeight) {

}
void COpenGLGraphicsEngine::SetFullScreen(bool bFullscreen) {

}

// == drawing
void COpenGLGraphicsEngine::ClearScreen() {

}

void COpenGLGraphicsEngine::BindVertexShaderConstantBuffer(int nSlot, BufferHandle_t hBuffer) {

}

void COpenGLGraphicsEngine::EnableAlphaBlending() {

}
void COpenGLGraphicsEngine::DisableAlphaBlending() {

}

void COpenGLGraphicsEngine::EnableDepthStencilState() {

}
void COpenGLGraphicsEngine::DisableDepthStencilState() {

}


void COpenGLGraphicsEngine::BindShaderProgram(HShaderProgram hProgram) {

}
void COpenGLGraphicsEngine::BindSampler(uint32_t uSlot, HSampler hSampler) {

}
void COpenGLGraphicsEngine::BindTexture(uint32_t uSlot, HTexture hTexture) {

}

void COpenGLGraphicsEngine::SetVertexBuffer(uint32_t uSlot, BufferHandle_t& vertexBuffer, unsigned int uStride, unsigned int uOffset) {

}
void COpenGLGraphicsEngine::SetIndexBuffer(BufferHandle_t& indexBuffer) {

}

void COpenGLGraphicsEngine::DrawIndexed(unsigned int indices) {

}
void COpenGLGraphicsEngine::DrawInstanced(unsigned int uVertexCountPerInstance, unsigned int uInstanceCount) {

}

bool COpenGLGraphicsEngine::Present() {
    glfwSwapBuffers(m_hWindow);
    return true;
}

// == 2d
ICanvas2D* COpenGLGraphicsEngine:: GetCanvas2D() {
    return this;
}

void COpenGLGraphicsEngine::RenderText(int x, int y, int w, int h, float color[3], std::string strText) {

}
void COpenGLGraphicsEngine::DrawRect(int x, int y, int w, int h, float color[3]) {

}
void COpenGLGraphicsEngine::DrawShape(Vector2* points, int pointCount, float color[3]) {

}
void COpenGLGraphicsEngine::FillShape(Vector2* points, int pointCount, float color[3]) {

}
void COpenGLGraphicsEngine::FillRect(int x, int y, int w, int h, float color[3]) {

}
void COpenGLGraphicsEngine::DrawImage(float x, float y, float w, float h, HBitmap hBmp) {

}
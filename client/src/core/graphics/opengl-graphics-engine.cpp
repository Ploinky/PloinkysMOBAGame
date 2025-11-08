#include "opengl-graphics-engine.h"

IGraphicsEngine* IGraphicsEngine::Create(HWindow hWindow, int nWidth, int nHeight) {
    return new COpenGLGraphicsEngine();
}


HShaderProgram COpenGLGraphicsEngine::LoadShaderProgram(std::string strShaderName, EVertexFormat eVertexFormat, std::vector<uint8_t> vecVsBytecode, std::vector<uint8_t> vecPsBytecode) {
    
}
HBitmap COpenGLGraphicsEngine::LoadBitmapImage(unsigned char* pImageData, int uWidth, int uHeight) {
    
}
HTexture COpenGLGraphicsEngine::LoadTexture(unsigned char* pImageData, int uWidth, int uHeight) {
    
}

// == buffer management
BufferHandle_t COpenGLGraphicsEngine::CreateVertexBuffer(void* pVertices, size_t uSize, int nCount) {
    
}
BufferHandle_t COpenGLGraphicsEngine::CreateIndexBuffer(uint32_t* pIndices, int nCount) {
    
}
BufferHandle_t COpenGLGraphicsEngine::CreateConstantBuffer(size_t uSize, void* pInitialData) {
    
}
BufferHandle_t COpenGLGraphicsEngine::CreateInstanceBuffer(void* instances, int instance_count, size_t size) {
    
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
    
}

// == 2d
ICanvas2D*COpenGLGraphicsEngine:: GetCanvas2D() {
    
}
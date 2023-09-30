#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <string>
#include "pmg_physics.h"
#include <map>
#include "GameObject.h"
#include "Shader.h"

namespace PMG {
    class Mesh;
    class Direct3D;
    class Camera;
    class Shader;


    class Renderer {
        public:
            ~Renderer();
            void Initialize(Direct3D* direct3D, int width, int height);
            void SetDimensions(int width, int height);
            void UpdateCameraMatrix();

            Camera* camera;
            Direct3D* direct3D;
            DirectX::XMFLOAT4X4 m_projMatrix;
            DirectX::XMFLOAT4X4 cameraMatrix;


            // ------------ NEW ------------
            std::vector<Shader*> m_shaders;

            
            void RenderText(int x, int y, int w, int h, float color[3], std::wstring text);
            void RenderText(int x, int y, int w, int h, std::wstring text);
            void DrawRect(int x, int y, int w, int h, float color[3]);
            void DrawShape(Physics::Vector2* points, int pointCount, float color[3]);
            void FillShape(Physics::Vector2* points, int pointCount, float color[3]);
            void FillRect(int x, int y, int w, int h, float color[3]);
            void UpdateBuffer(ID3D11Buffer* buffer, const void* src, size_t size);

            void BindShader(ShaderType type);

            template<typename T>
            void UpdateShaderConst(T const_data);
            
            void SetVertexBuffer(ID3D11Buffer* buffer, UINT stride, UINT offset);
            void SetVertexBuffers(ID3D11Buffer** buffers, UINT* stride, UINT* offset);
            void SetIndexBuffer(ID3D11Buffer* buffer);
            void SetShaderResource(UINT start_slot, UINT num_views, ID3D11ShaderResourceView* resource);
            void DrawIndexed(int count);
            void DrawInstanced(int vertex_count, int instance_count);

            void EnableAlphaBlending();
            void DisableAlphaBlending();
            
        private:
            int m_width;
            int m_height;

            std::map<std::string, Mesh*> meshes_;
    };
}
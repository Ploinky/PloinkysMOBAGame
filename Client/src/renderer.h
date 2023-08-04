#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <string>
#include "physics.h"

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
            mat_t m_projMatrix;
            DirectX::XMFLOAT4X4 cameraMatrix;


            // ------------ NEW ------------
            std::vector<Shader*> m_shaders;

            
            void RenderMeshes(std::vector<Mesh*> meshes);
            void RenderText(int x, int y, int w, int h, float color[3], std::wstring text);
            void RenderText(int x, int y, int w, int h, std::wstring text);
            void DrawRect(int x, int y, int w, int h, float color[3]);
            void DrawShape(vec2_t* points, int pointCount, float color[3]);
            void FillShape(vec2_t* points, int pointCount, float color[3]);
            void FillRect(int x, int y, int w, int h, float color[3]);
            void UpdateBuffer(ID3D11Buffer* buffer, const void* src, size_t size);
            
        private:
            int m_width;
            int m_height;
    };
}
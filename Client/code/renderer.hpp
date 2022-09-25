#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

namespace PMG {
    class Mesh;
    class Direct3D;
    class Camera;
    class Shader;


    class Renderer {
        public:
            ~Renderer();
            void Initialize(Direct3D* direct3D, int width, int height);
            void SetAspectRatio(float aspect);
            void UpdateCameraMatrix();

            void TestIntersect(int mx, int my, float* outX, float* outY);

            Camera* camera;
            Direct3D* direct3D;
            DirectX::XMFLOAT4X4 projMatrix;
            DirectX::XMFLOAT4X4 cameraMatrix;


            // ------------ NEW ------------
            std::vector<Shader*> m_shaders;
            
            void RenderMeshes(std::vector<Mesh*> meshes);
            void UpdateBuffer(ID3D11Buffer* buffer, const void* src, size_t size);
    };
}
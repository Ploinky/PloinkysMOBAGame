#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

namespace P3D {
    class Mesh;
    class Direct3D;
    class Camera;

    struct object_constant_buffer {
        DirectX::XMFLOAT4X4 modelMatrix;
    };

    struct frame_constant_buffer {
        DirectX::XMFLOAT4X4 projMatrix;
        DirectX::XMFLOAT4X4 cameraMatrix;
    };

    class Renderer {
        public:
            ~Renderer();
            void Initialize(Direct3D* direct3D, int width, int height);
            void Render(Mesh* model);
            void SetAspectRatio(float aspect);
            void UpdateCameraMatrix();

            Camera* camera;
            Direct3D* direct3D;
            ID3D11InputLayout* inputLayout;
            ID3D11VertexShader* vertexShader;
            ID3D11PixelShader* pixelShader;
            ID3D11Buffer* constantBuffer;
            ID3D11Buffer* frameConstantBuffer;
            object_constant_buffer vsConstData;
            frame_constant_buffer frameConstBuffer;

        private:
            void UpdateFrameConstantBuffer();
            void UpdateModelConstantBuffer();
    };
}
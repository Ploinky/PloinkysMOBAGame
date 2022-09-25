#pragma once

#include "d3d11.h"
#include <stdint.h>
#include <DirectXMath.h>
#include "shader.hpp"
#include <string>

namespace PMG {
    class Direct3D;
    class Vector3D;
    class Vertex;

    class Mesh {
        public:
            Vertex* vertices;
            uint32_t vertexCount;
            unsigned int* indices;
            uint32_t indexCount;
            ID3D11Buffer* vertexBuffer;
            ID3D11Buffer* indexBuffer;
            DirectX::XMFLOAT3 position = DirectX::XMFLOAT3();
            DirectX::XMFLOAT3 rotation = DirectX::XMFLOAT3();
            ShaderType m_shaderType;
            
            unsigned long unit;

            Mesh() : m_shaderType(ShaderType::COLOR) {};
            Mesh(ShaderType shaderType) : m_shaderType(shaderType) {};
            ~Mesh();
            virtual bool Initialize(Direct3D* direct3D);
            bool IsInitialized();
            bool initialized = false;
    };

    class TextureMesh : public Mesh {
    public:
        std::string m_textureFileName;
        ID3D11ShaderResourceView* m_texture;
        ShaderType m_shaderType = ShaderType::TEXTURE;
        texture_shader_vertex_t* vertices;

        TextureMesh() : Mesh(ShaderType::TEXTURE) {};

        bool Initialize(Direct3D* direct3D);
    };
}
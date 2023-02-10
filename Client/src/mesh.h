#pragma once

#include "d3d11.h"
#include <stdint.h>
#include <DirectXMath.h>
#include "shader.h"
#include <string>
#include "physics.h"

namespace PMG {
    class Direct3D;
    class Vector3D;
    class Vertex;

    class Mesh {
        public:
            Vertex* vertices = 0;
            uint32_t vertexCount = 0;
            unsigned int* indices = 0;
            uint32_t indexCount = 0;
            ID3D11Buffer* vertexBuffer = 0;
            ID3D11Buffer* indexBuffer = 0;
            vec3_t position = vec3_t { 0, 0, 0 };
            vec3_t rotation = vec3_t { 0, 0, 0 };
            ShaderType m_shaderType = ShaderType::COLOR;
            
            unsigned long unit = 0;

            Mesh() : m_shaderType(ShaderType::COLOR) {};
            Mesh(ShaderType shaderType) : m_shaderType(shaderType) {};
            virtual ~Mesh();
            virtual bool Initialize(Direct3D* direct3D);
            bool IsInitialized();
            bool initialized = false;
    };

    class TextureMesh : public Mesh {
    public:
        std::string m_textureFileName = "";
        ID3D11ShaderResourceView* m_texture = 0;
        ShaderType m_shaderType = ShaderType::TEXTURE;
        texture_shader_vertex_t* vertices = 0;

        TextureMesh() : Mesh(ShaderType::TEXTURE) {};
        ~TextureMesh();

        bool Initialize(Direct3D* direct3D);
    };
}
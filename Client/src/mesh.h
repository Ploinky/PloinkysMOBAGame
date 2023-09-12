#pragma once

#include "d3d11.h"
#include <stdint.h>
#include <DirectXMath.h>
#include "shader.h"
#include <string>
#include "pmg_physics.h"
#include "armature.h"
#include <map>
#include "animation.h"

namespace PMG {
    class Direct3D;
    class Vector3D;
    class Vertex;

    class Mesh {
        public:
            color_shader_vertex_t* vertices = 0;
            uint32_t vertexCount = 0;
            unsigned int* indices = 0;
            uint32_t indexCount = 0;
            ID3D11Buffer* vertexBuffer = 0;
            ID3D11Buffer* indexBuffer = 0;
            Physics::Vector3 position = Physics::Vector3{ 0, 0, 0 };
            Physics::Vector3 rotation = Physics::Vector3{ 0, 0, 0 };
            ShaderType m_shaderType = ShaderType::COLOR;
            
            unsigned long unit = 0;

            Mesh() : m_shaderType(ShaderType::COLOR) {};
            Mesh(ShaderType shaderType) : m_shaderType(shaderType) {};
            virtual ~Mesh();
            virtual bool Initialize(Direct3D* direct3D);
            bool IsInitialized();
            bool initialized = false;

            static Mesh* LoadMesh(std::string file_name, std::string texture_file_name);
            static Mesh* LoadSkinnedTexturedMesh(std::string file_name, std::string texture_file_name);
    };

    class TextureMesh : public Mesh {
    public:
        std::string m_textureFileName = "";
        ID3D11ShaderResourceView* m_texture = 0;
        texture_shader_vertex_t* vertices = 0;

        TextureMesh() : Mesh(ShaderType::TEXTURE) {};
        ~TextureMesh();

        bool Initialize(Direct3D* direct3D);
    };

    class SkinnedTexturedMesh : public Mesh {
    public:
        std::string m_textureFileName = "";
        ID3D11ShaderResourceView* m_texture = 0;
        Armature* armature = nullptr;
        std::map<std::string, Animation*> animations;
        skinned_textured_shader_vertex_t* vertices = 0;
        DirectX::XMMATRIX animation_palette[256];
        Animation* current_animation = nullptr;
        double current_animation_time = 0;

        SkinnedTexturedMesh() : Mesh(ShaderType::SKINNED_TEXTURED) {};
        ~SkinnedTexturedMesh();

        bool Initialize(Direct3D* direct3D);
        void CalculateMatrixPalette();
        double PlayAnimation(std::string animation_name, double time);
    };
}
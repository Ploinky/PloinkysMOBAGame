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
#include "renderer.h"
#include "logger.h"

namespace PMG {
    class Direct3D;
    class Vector3D;
    class Vertex;

    class Mesh {
    public:
        Mesh() : m_shaderType(ShaderType::COLOR) {};
        Mesh(ShaderType shaderType) : m_shaderType(shaderType) {};
        virtual ~Mesh();

        virtual void Update(double dt) {};
        virtual void PlayAnimation(std::string animation_name) {
            Logger::Err("Attempting to animate mesh that has no animations");
        };
        virtual void StopAnimation() {
            Logger::Err("Attempting to animate mesh that has no animations");
        };
        virtual void Render(Renderer* renderer);

        unsigned long unit = 0;

        virtual bool Initialize(Direct3D* direct3D);
        bool IsInitialized();

        // TODO privatize
        uint32_t vertexCount = 0;
        unsigned int* indices = 0;
        uint32_t indexCount = 0;
        Physics::Vector3 position = Physics::Vector3{ 0, 0, 0 };
        Physics::Vector3 rotation = Physics::Vector3{ 0, 0, 0 };

    protected:
        ID3D11Buffer* vertexBuffer = 0;
        ID3D11Buffer* indexBuffer = 0;
        bool initialized = false;

    private:
        color_shader_vertex_t* vertices = 0;
        ShaderType m_shaderType = ShaderType::COLOR;
    };

    class TextureMesh : public Mesh {
    public:
        TextureMesh() : Mesh(ShaderType::TEXTURE) {};
        ~TextureMesh();

        static TextureMesh* Load(std::string mesh_name, Direct3D* direct3D);

        virtual void Update(double dt) override {};
        virtual void PlayAnimation(std::string animation_name) override {
            Logger::Err("Attempting to animate mesh that has no animations");
        };
        virtual void StopAnimation() override {
            Logger::Err("Attempting to animate mesh that has no animations");
        };
        virtual void Render(Renderer* renderer) override;

        // TODO privatize
        std::string m_textureFileName = "";
        ID3D11ShaderResourceView* m_texture = 0;
        texture_shader_vertex_t* vertices = 0;

    private:
        bool Initialize(Direct3D* direct3D);
    };

    class SkinnedTexturedMesh : public Mesh {
    public:
        SkinnedTexturedMesh() : Mesh(ShaderType::SKINNED_TEXTURED) {};
        ~SkinnedTexturedMesh();

        virtual void Update(double dt) override;
        virtual void PlayAnimation(std::string animation_name) override;
        virtual void StopAnimation() override;
        virtual void Render(Renderer* renderer) override;

        bool Initialize(Direct3D* direct3D);

        static SkinnedTexturedMesh* Load(std::string mesh_name, Direct3D* direct3D);
    private:
        std::string m_textureFileName = "";
        ID3D11ShaderResourceView* m_texture = 0;
        Armature* armature = nullptr;
        std::map<std::string, Animation*> animations;
        skinned_textured_shader_vertex_t* vertices = 0;
        DirectX::XMMATRIX animation_palette[256];
        Animation* current_animation = nullptr;
        double current_animation_time = 0;
        void CalculateMatrixPalette();
    };
}
#include "logger.h"
#include <fstream>
#include "DDSTextureLoader11.h"
#include "mesh.h"
#include "direct3d.h"

namespace PMG {
    SkinnedTexturedMesh* SkinnedTexturedMesh::Load(std::string mesh_name, Direct3D* direct3D) {
        std::ifstream file(std::string().append(mesh_name).append(".p3d"), std::ios_base::binary | std::ios_base::in);

        if (!file.is_open()) {
            Logger::Err("Could not open map file.");
            return nullptr;
        }

        SkinnedTexturedMesh* mesh = new SkinnedTexturedMesh();
        int currIndex = 0;
        int currVertex = 0;

        std::string correct_magic = "p3d";
        char magic[4]{ 0, 0, 0, 0 };
        file.read(magic, sizeof(char) * 3);
        std::string magic_string(magic);
        if (magic_string.compare(correct_magic) != 0) {
            Logger::Err("Bad magic string!");
            return nullptr;
        }

        int version;
        file.read((char*)&version, sizeof(int));

        if (version != 1) {
            Logger::Err("Bad version!");
            return nullptr;
        }

        int vertex_count;
        file.read((char*)&vertex_count, sizeof(int));
        mesh->vertexCount = vertex_count;

        mesh->vertices = new skinned_textured_shader_vertex_t[mesh->vertexCount];

        for (unsigned int i = 0; i < mesh->vertexCount; i++) {
            file.read((char*)&mesh->vertices[i].position[0], sizeof(float));
            file.read((char*)&mesh->vertices[i].position[1], sizeof(float));
            file.read((char*)&mesh->vertices[i].position[2], sizeof(float));
            file.read((char*)&mesh->vertices[i].normal[0], sizeof(float));
            file.read((char*)&mesh->vertices[i].normal[1], sizeof(float));
            file.read((char*)&mesh->vertices[i].normal[2], sizeof(float));
            file.read((char*)&mesh->vertices[i].texCoord[0], sizeof(float));
            file.read((char*)&mesh->vertices[i].texCoord[1], sizeof(float));
            file.read((char*)&mesh->vertices[i].bone_indices[0], sizeof(float));
            file.read((char*)&mesh->vertices[i].bone_indices[1], sizeof(float));
            file.read((char*)&mesh->vertices[i].bone_indices[2], sizeof(float));
            file.read((char*)&mesh->vertices[i].bone_indices[3], sizeof(float));
            file.read((char*)&mesh->vertices[i].bone_weights[0], sizeof(float));
            file.read((char*)&mesh->vertices[i].bone_weights[1], sizeof(float));
            file.read((char*)&mesh->vertices[i].bone_weights[2], sizeof(float));
            file.read((char*)&mesh->vertices[i].bone_weights[3], sizeof(float));
        }

        int index_count;
        file.read((char*)&index_count, sizeof(int));
        mesh->indexCount = index_count;

        mesh->indices = new unsigned int[mesh->indexCount];

        for (unsigned int i = 0; i < mesh->indexCount; i++) {
            file.read((char*)&mesh->indices[i], sizeof(int));
        }

        int animation_count;
        file.read((char*)&animation_count, sizeof(int));

        for (int i = 0; i < animation_count; i++) {
            int buf_len;
            file.read((char*)&buf_len, sizeof(int));

            char* name = new char[buf_len + 1] {0};

            file.read(name, buf_len);

            std::string anim_file_name = std::string().append(mesh_name).append("_").append(name).append(".p3d_anim");

            Animation* animation = Animation::LoadAnimation(anim_file_name);

            if (animation != nullptr) {
                mesh->animations.emplace(std::string(name), animation);
            }

            delete[] name;
        }
        mesh->m_textureFileName = std::string().append(mesh_name).append(".dds");

        Armature* armature = Armature::LoadArmature(std::string().append(mesh_name).append(".p3d_skn"));

        if (armature != nullptr) {
            armature->ComputeGlobalInverseBindPoses();
            mesh->armature = armature;
        }

        mesh->Initialize(direct3D);

        return mesh;
    }

    void SkinnedTexturedMesh::Update(double dt) {
        if (current_animation == nullptr) {
            return;
        }

        current_animation_time += dt;


        if (current_animation_time > current_animation->duration * 1000) {
            current_animation_time = 0;
        }

        CalculateMatrixPalette();
    }

    void SkinnedTexturedMesh::PlayAnimation(std::string animation_name) {
        auto it = animations.find(animation_name.c_str());

        if (it == animations.end() || it->second == nullptr) {
            Logger::Err("Attempt to play missing animation");
            StopAnimation();
            return;
        }

        if (current_animation == it->second) {
            return;
        }

        current_animation = it->second;
        current_animation_time = 0;
    }

    void SkinnedTexturedMesh::StopAnimation() {
        current_animation = nullptr;
        current_animation_time = 0;
        CalculateMatrixPalette();
    }

    void SkinnedTexturedMesh::Render(Renderer* renderer) {
        renderer->BindShader(ShaderType::SKINNED_TEXTURED);

        texture_shader_frame_const_t data{};
        data.cameraMatrix = renderer->cameraMatrix;
        data.projMatrix = renderer->m_projMatrix;

        renderer->UpdateShaderConst<texture_shader_frame_const_t>(data);

        texture_shader_model_const_t model_data{};
        DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(rotation.x),
            DirectX::XMConvertToRadians(rotation.y),
            DirectX::XMConvertToRadians(rotation.z));
        DirectX::XMMATRIX transMat = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
        DirectX::XMStoreFloat4x4(&model_data.modelMatrix, DirectX::XMMatrixTranspose(rotMat * transMat));

        for (int i = 0; i < armature->bones.size(); i++) {
            DirectX::XMStoreFloat4x4(&model_data.animation_palette[i], DirectX::XMMatrixTranspose(animation_palette[i]));
        }

        renderer->UpdateShaderConst<texture_shader_model_const_t>(model_data);

        // Render this specific model
        UINT stride = sizeof(skinned_textured_shader_vertex_t);
        UINT offset = 0;
        renderer->SetVertexBuffer(vertexBuffer, stride, offset);
        renderer->SetIndexBuffer(indexBuffer);
        renderer->SetShaderResource(0, 1, m_texture);
        renderer->DrawIndexed(indexCount);
    }


    SkinnedTexturedMesh::~SkinnedTexturedMesh() {
        if (m_texture != nullptr) {
            m_texture->Release();
            m_texture = nullptr;
        }

        for (auto it : animations) {
            delete it.second;
        }

        animations.clear();

        if (armature != nullptr) {
            delete armature;
            armature = nullptr;
        }
    }

    bool SkinnedTexturedMesh::Initialize(Direct3D* direct3D) {
        vertexBuffer = direct3D->CreateVertexBuffer(vertices, vertexCount, sizeof(skinned_textured_shader_vertex_t) * vertexCount);

        if (vertexBuffer == nullptr) {
            initialized = false;
            return false;
        }

        indexBuffer = direct3D->CreateIndexBuffer(indices, indexCount);

        if (indexBuffer == nullptr) {
            initialized = false;
            return false;
        }

        if (m_textureFileName.length() == 0) {
            initialized = true;
            return true;
        }

        HRESULT hr = DirectX::CreateDDSTextureFromFile(direct3D->device, std::wstring(m_textureFileName.begin(), m_textureFileName.end()).c_str(), NULL, &m_texture, 0, NULL);

        if (FAILED(hr)) {
            Logger::Err("Could not create texture from dds file");
            return false;
        }

        CalculateMatrixPalette();

        initialized = true;

        return initialized;
    }

    void SkinnedTexturedMesh::CalculateMatrixPalette() {
        std::vector<DirectX::XMMATRIX> current_poses;

        if (current_animation == nullptr) {
            current_poses.resize(armature->bones.size());

            current_poses[0] = armature->bones[0].bind_pose.ToMatrix();

            for (int bone = 1; bone < armature->bones.size(); bone++) {
                Physics::mat_t mat = Physics::mat_t::Identity();

                current_poses[bone] = armature->bones[bone].bind_pose.ToMatrix() * current_poses[armature->bones[bone].parent_index];
            }
        }
        else {
            current_animation->GetGlobalPoseAtTime(current_poses, armature, current_animation_time);
        }

        for (int i = 0; i < armature->bones.size(); i++) {
            animation_palette[i] = armature->global_inverse_bind_poses[i] * current_poses[i];
        }
    }
}
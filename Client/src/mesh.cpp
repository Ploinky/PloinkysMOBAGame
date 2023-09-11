#include "mesh.h"
#include "vertex.h"
#include "direct3d.h"
#include "DDSTextureLoader11.h"
#include <fstream>
#include "logger.h"
#include "util.h"

namespace PMG {
    Mesh::~Mesh() {
        if (initialized) {
            vertexBuffer->Release();
            indexBuffer->Release();
        }
    }

    bool Mesh::Initialize(Direct3D* direct3D) {
        vertexBuffer = direct3D->CreateVertexBuffer(vertices, vertexCount, sizeof(color_shader_vertex_t) * vertexCount);

        if(vertexBuffer == nullptr) {
            initialized = false;
            return false;
        }

        indexBuffer = direct3D->CreateIndexBuffer(indices, indexCount);

        if(indexBuffer == nullptr) {
            initialized = false;
            return false;
        }

        initialized = true;

        return initialized;
    }

    bool Mesh::IsInitialized() {
        return initialized;
    }

    Mesh* Mesh::LoadSkinnedTexturedMesh(std::string file_name, std::string texture_file_name) {
        std::ifstream file(file_name, std::ios_base::binary | std::ios_base::in);

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

        for (int i = 0; i < mesh->vertexCount; i++) {
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

        for (int i = 0; i < mesh->indexCount; i++) {
            file.read((char*)&mesh->indices[i], sizeof(int));
        }

        mesh->m_textureFileName = texture_file_name;

        // try to load armature?
        Armature* armature = Armature::LoadArmature(file_name.append("_skn"));

        if (armature != nullptr) {
            armature->ComputeGlobalInverseBindPoses();
            mesh->armature = armature;
        }

        return mesh;
    }

    Mesh* Mesh::LoadMesh(std::string file_name, std::string texture_file_name) {
        std::ifstream file(file_name, std::ios_base::binary | std::ios_base::in);

        if (!file.is_open()) {
            Logger::Err("Could not open map file.");
            return nullptr;
        }

        TextureMesh* mesh = new TextureMesh();
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

        mesh->vertices = new texture_shader_vertex_t[mesh->vertexCount];

        for (int i = 0; i < mesh->vertexCount; i++) {
            file.read((char*)&mesh->vertices[i].position[0], sizeof(float));
            file.read((char*)&mesh->vertices[i].position[1], sizeof(float));
            file.read((char*)&mesh->vertices[i].position[2], sizeof(float));
            file.read((char*)&mesh->vertices[i].texCoord[0], sizeof(float));
            file.read((char*)&mesh->vertices[i].texCoord[1], sizeof(float));
        }

        int index_count;
        file.read((char*)&index_count, sizeof(int));
        mesh->indexCount = index_count;

        mesh->indices = new unsigned int[mesh->indexCount];

        for (int i = 0; i < mesh->indexCount; i++) {
            file.read((char*)&mesh->indices[i], sizeof(int));
        }

        mesh->m_textureFileName = texture_file_name;

        return mesh;
    }

    TextureMesh::~TextureMesh() {
        if (m_texture != nullptr) {
            m_texture->Release();
            m_texture = nullptr;
        }
    }

    bool TextureMesh::Initialize(Direct3D* direct3D) {
        vertexBuffer = direct3D->CreateVertexBuffer(vertices, vertexCount, sizeof(texture_shader_vertex_t) * vertexCount);

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
        /*
        char* data = new char[1024 * 1024 * 4]{};

        for (UINT row = 0; row < 1024; row++)
        {
            UINT rowStart = row * 1024;
            for (UINT col = 0; col < 1024; col++)
            {
                UINT colStart = col * 4;
                data[rowStart + colStart + 0] = 255; // Red
                data[rowStart + colStart + 1] = 0; // Green
                data[rowStart + colStart + 2] = 0;  // Blue
                data[rowStart + colStart + 3] = 255;  // Alpha
            }
        }

        for (int i = 0; i < 1024 * 1024 * 4; i += 4) {
            data[i] = 255;
            data[i + 1] = 0; // Green
            data[i + 2] = 0;  // Blue
            data[i + 3] = 255;  // Alpha
        }
        D3D11_SUBRESOURCE_DATA subresource_data{};
        subresource_data.pSysMem = data;
        subresource_data.SysMemPitch = 1024 * sizeof(char) * 4;
        subresource_data.SysMemSlicePitch= 0;


        D3D11_TEXTURE2D_DESC descs[1];
        D3D11_TEXTURE2D_DESC desc{};
        desc.Width = 1024;
        desc.Height = 1024;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        descs[0] = desc;

        ID3D11Texture2D* pTexture = NULL;
        HRESULT hr = direct3D->device->CreateTexture2D(descs, &subresource_data, &pTexture);
        if (FAILED(hr)) {
            printf("Could not create texture from dds file");
            return false;
        }
        hr = direct3D->device->CreateShaderResourceView(pTexture, nullptr, &m_texture);
        if (FAILED(hr)) {
            printf("Could not create texture from dds file");
            return false;
        }
        */
        
        HRESULT hr = DirectX::CreateDDSTextureFromFile(direct3D->device, std::wstring(m_textureFileName.begin(), m_textureFileName.end()).c_str(), NULL, &m_texture, 0, NULL);

        if (FAILED(hr)) {
            printf("Could not create texture from dds file");
            return false;
        }

        initialized = true;

        return initialized;
    }


    SkinnedTexturedMesh::~SkinnedTexturedMesh() {
        if (m_texture != nullptr) {
            m_texture->Release();
            m_texture = nullptr;
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
        /*
        char* data = new char[1024 * 1024 * 4]{};

        for (UINT row = 0; row < 1024; row++)
        {
            UINT rowStart = row * 1024;
            for (UINT col = 0; col < 1024; col++)
            {
                UINT colStart = col * 4;
                data[rowStart + colStart + 0] = 255; // Red
                data[rowStart + colStart + 1] = 0; // Green
                data[rowStart + colStart + 2] = 0;  // Blue
                data[rowStart + colStart + 3] = 255;  // Alpha
            }
        }

        for (int i = 0; i < 1024 * 1024 * 4; i += 4) {
            data[i] = 255;
            data[i + 1] = 0; // Green
            data[i + 2] = 0;  // Blue
            data[i + 3] = 255;  // Alpha
        }
        D3D11_SUBRESOURCE_DATA subresource_data{};
        subresource_data.pSysMem = data;
        subresource_data.SysMemPitch = 1024 * sizeof(char) * 4;
        subresource_data.SysMemSlicePitch= 0;


        D3D11_TEXTURE2D_DESC descs[1];
        D3D11_TEXTURE2D_DESC desc{};
        desc.Width = 1024;
        desc.Height = 1024;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        descs[0] = desc;

        ID3D11Texture2D* pTexture = NULL;
        HRESULT hr = direct3D->device->CreateTexture2D(descs, &subresource_data, &pTexture);
        if (FAILED(hr)) {
            printf("Could not create texture from dds file");
            return false;
        }
        hr = direct3D->device->CreateShaderResourceView(pTexture, nullptr, &m_texture);
        if (FAILED(hr)) {
            printf("Could not create texture from dds file");
            return false;
        }
        */

        HRESULT hr = DirectX::CreateDDSTextureFromFile(direct3D->device, std::wstring(m_textureFileName.begin(), m_textureFileName.end()).c_str(), NULL, &m_texture, 0, NULL);

        if (FAILED(hr)) {
            printf("Could not create texture from dds file");
            return false;
        }

        initialized = true;

        return initialized;
    }

    void SkinnedTexturedMesh::CalculateMatrixPalette() {
        std::vector<Physics::mat_t> current_poses;

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

    double SkinnedTexturedMesh::PlayAnimation(std::string animation_name, double time) {
        current_animation = animations.find(animation_name)->second;
        current_animation_time = time;

        CalculateMatrixPalette();

        return current_animation->duration;
    }
}
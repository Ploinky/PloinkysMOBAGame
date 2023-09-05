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

        HRESULT hr = DirectX::CreateDDSTextureFromFile(direct3D->device, std::wstring(m_textureFileName.begin(), m_textureFileName.end()).c_str(), NULL, &m_texture, 0, NULL);

        if (FAILED(hr)) {
            printf("Could not create texture from dds file");
            return false;
        }

        initialized = true;

        return initialized;
    }
}
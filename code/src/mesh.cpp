#include "mesh.hpp"
#include "vertex.hpp"
#include "direct3d.hpp"
#include "DDSTextureLoader11.h"

namespace P3D {
    Mesh::~Mesh() {
        vertexBuffer->Release();
        indexBuffer->Release();
    }

    bool Mesh::Initialize(Direct3D* direct3D) {
        vertexBuffer = direct3D->CreateVertexBuffer(vertices, vertexCount, sizeof(Vertex) * vertexCount);

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

        HRESULT hr = DirectX::CreateDDSTextureFromFile(direct3D->device, std::wstring(m_textureFileName.begin(), m_textureFileName.end()).c_str(), NULL, &m_texture, 0, NULL);

        if (FAILED(hr)) {
            printf("Could not create texture from dds file");
            return false;
        }

        initialized = true;

        return initialized;
    }
}
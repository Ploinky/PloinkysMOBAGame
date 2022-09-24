#include "renderer.hpp"
#include "mesh.hpp"
#include "logger.hpp"
#include "vertex.hpp"
#include "direct3d.hpp"
#include "util.hpp"
#include "camera.hpp"
#include "shader.hpp"

namespace P3D {
    static DirectX::XMMATRIX perspMatrix;

    Renderer::~Renderer() {
    }

    void Renderer::Initialize(Direct3D* direct3D, int width, int height) {
        this->direct3D = direct3D;
        camera = new Camera();

        perspMatrix = DirectX::XMMatrixTranspose(DirectX::XMMatrixPerspectiveFovLH(
            DirectX::XMConvertToRadians(camera->fov), (float) width / (float) height, camera->nearClip, camera->farClip));

        // Where to set this?
        direct3D->context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // Set initial constant matrix values
        DirectX::XMStoreFloat4x4(&projMatrix, perspMatrix);
        DirectX::XMStoreFloat4x4(&cameraMatrix, DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, DirectX::XMMatrixTranslation(
            camera->position.x, camera->position.y, camera->position.z))));

        // ------------ NEW ------------
        ColorShader* colorShader = new ColorShader();
        colorShader->Initialize(direct3D);

        m_shaders.push_back(colorShader);

        TextureShader* textureShader = new TextureShader();
        textureShader->Initialize(direct3D);

        m_shaders.push_back(textureShader);
    }

    void Renderer::SetAspectRatio(float aspect) {
        printf("aspect: %f\r\n", aspect);
        perspMatrix = DirectX::XMMatrixTranspose(DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(camera->fov), aspect, camera->nearClip, camera->farClip));
        DirectX::XMStoreFloat4x4(&projMatrix, perspMatrix);
    }

    void Renderer::UpdateCameraMatrix() {
        DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(camera->rotation.x),
                        DirectX::XMConvertToRadians(camera->rotation.y),
                        DirectX::XMConvertToRadians(camera->rotation.z));
        
        DirectX::XMMATRIX transMat =  DirectX::XMMatrixTranslation(camera->position.x, camera->position.y, camera->position.z);

        DirectX::XMStoreFloat4x4(&cameraMatrix, DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, rotMat * transMat)));
    }

    void Renderer::TestIntersect(int mx, int my, float* outX, float* outZ) {
        DirectX::XMMATRIX projMat = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&projMatrix));
        DirectX::XMMATRIX camMat = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&cameraMatrix));
        DirectX::XMMATRIX worldMat = DirectX::XMMatrixTranspose(DirectX::XMMatrixTranslation(0, 0, 0));

        DirectX::XMMATRIX mat = projMat * camMat;
        DirectX::XMMATRIX invMat = DirectX::XMMatrixInverse(nullptr, mat);
        DirectX::XMVECTOR screenCoords = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(mx, my, 0));
        DirectX::XMVECTOR endCoords = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(mx, my, 1.0f));
        DirectX::XMVECTOR rayOriginV = DirectX::XMVector3Unproject(screenCoords, 0, 0, 1024, 768, 0.0f, 1.0f, projMat, camMat, worldMat);
        DirectX::XMVECTOR rayEndV = DirectX::XMVector3Unproject(endCoords, 0, 0, 1024, 768, 0.0f, 1.0f, projMat, camMat, worldMat);
        DirectX::XMFLOAT3 rayOrigin;
        DirectX::XMStoreFloat3(&rayOrigin, DirectX::XMVectorScale(rayOriginV, DirectX::XMVectorGetW(rayOriginV)));
        DirectX::XMFLOAT3 rayEnd;
        DirectX::XMStoreFloat3(&rayEnd, DirectX::XMVectorScale(rayEndV, DirectX::XMVectorGetW(rayEndV)));
        DirectX::XMFLOAT3 normal(0.0f, 1.0f, 0.0f);
        DirectX::XMFLOAT3 origin(0.0f, 0.0f, 0.0f);
        DirectX::XMVECTOR normalV = DirectX::XMLoadFloat3(&normal);
        DirectX::XMVECTOR originV = DirectX::XMLoadFloat3(&origin);
        DirectX::XMVECTOR rayV = DirectX::XMVectorSubtract(rayEndV, rayOriginV);
        DirectX::XMFLOAT3 ray;
        DirectX::XMStoreFloat3(&ray, rayV);


        float denom = DirectX::XMVectorGetX(DirectX::XMVector3Dot(normalV, rayV));

        if (abs(denom) > 0.0001f) {
            float t = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMVectorSubtract(originV, rayOriginV), normalV)) / denom;
            *outX = rayOrigin.x + ray.x * t;
            *outZ = rayOrigin.z + ray.z * t;
        }
    }

    void Renderer::UpdateBuffer(ID3D11Buffer* buffer, const void* src, size_t size) {
        D3D11_MAPPED_SUBRESOURCE mappedResource = { 0 };
        direct3D->context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        memcpy(mappedResource.pData, src, size);
        direct3D->context->Unmap(buffer, 0);
    }

    void Renderer::RenderMeshes(std::vector<Mesh*> meshes) {
        // Maybe update some frame-wide constant values?
        // For example camera and projection matrix?
        
        // Iterate over the shaders (optimally only those we need)
        // For each shader
        for (Shader* shader : m_shaders) {
            // Bind the shader
            direct3D->context->VSSetShader(shader->m_vertexShader, 0, 0);
            direct3D->context->PSSetShader(shader->m_pixelShader, 0, 0);
            direct3D->context->IASetInputLayout(shader->m_inputLayout);

            switch (shader->m_type) {
                case ShaderType::COLOR: {
                    ColorShader* colorShader = (ColorShader*)shader;
                    // Do some stuff to update the frame constants for this shader

                    // Update and set frame constant buffer
                    colorShader->m_frameConstData.cameraMatrix = cameraMatrix;
                    colorShader->m_frameConstData.projMatrix = projMatrix;
                    UpdateBuffer(colorShader->m_frameConstBuffer, &colorShader->m_frameConstData, sizeof(colorShader->m_frameConstData));
                    direct3D->context->VSSetConstantBuffers(0, 1, &colorShader->m_frameConstBuffer);

                    // For each model in the scene
                    for (Mesh* mesh : meshes) {
                        // That uses this shader!
                        if (mesh->m_shaderType != shader->m_type) {
                            continue;
                        }

                        // Lazy initialize the model's Direct3D resources
                        if (!mesh->IsInitialized()) {
                            if (!mesh->Initialize(direct3D)) {
                                Logger::Err("Failed to initialize 3D model!");
                                // Maybe quit here?
                                return;
                            }
                        }

                        // Do some stuff to update the model specific constants that this specific shader uses
                        // Update model constant buffer
                        DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(mesh->rotation.x),
                            DirectX::XMConvertToRadians(mesh->rotation.y),
                            DirectX::XMConvertToRadians(mesh->rotation.z));
                        DirectX::XMMATRIX transMat = DirectX::XMMatrixTranslation(mesh->position.x, mesh->position.y, mesh->position.z);
                        DirectX::XMStoreFloat4x4(&colorShader->m_modelConstData.modelMatrix, DirectX::XMMatrixTranspose(rotMat * transMat));

                        transMat = DirectX::XMMatrixAffineTransformation(DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f), DirectX::XMVectorSet(0, 0, 0, 1.0f),
                            DirectX::XMQuaternionRotationMatrix(DirectX::XMMatrixRotationRollPitchYaw(mesh->rotation.x * 0.0174532925f, mesh->rotation.y * 0.0174532925f, mesh->rotation.z * 0.0174532925f)),
                            DirectX::XMVectorSet(mesh->position.x, mesh->position.y, mesh->position.z, 1.0f));

                        UpdateBuffer(colorShader->m_modelConstBuffer, &colorShader->m_modelConstData, sizeof(colorShader->m_modelConstData));
                        direct3D->context->VSSetConstantBuffers(1, 1, &colorShader->m_modelConstBuffer);

                        // Render this specific model
                        UINT stride = sizeof(Vertex);
                        UINT offset = 0;
                        direct3D->context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                        direct3D->context->IASetVertexBuffers(0, 1, &mesh->vertexBuffer, &stride, &offset);
                        direct3D->context->IASetIndexBuffer(mesh->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
                        direct3D->context->DrawIndexed(mesh->indexCount, 0, 0);
                    }
                    break;
                }
                case ShaderType::TEXTURE: {
                    TextureShader* textureShader = (TextureShader*)shader;
                    // Do some stuff to update the frame constants for this shader

                    // Update and set frame constant buffer
                    textureShader->m_frameConstData.cameraMatrix = cameraMatrix;
                    textureShader->m_frameConstData.projMatrix = projMatrix;
                    UpdateBuffer(textureShader->m_frameConstBuffer, &textureShader->m_frameConstData, sizeof(textureShader->m_frameConstData));
                    direct3D->context->VSSetConstantBuffers(0, 1, &textureShader->m_frameConstBuffer);

                    // For each model in the scene
                    for (Mesh* mesh : meshes) {
                        // That uses this shader!
                        if (mesh->m_shaderType != shader->m_type) {
                            continue;
                        }

                        // Lazy initialize the model's Direct3D resources
                        if (!mesh->IsInitialized()) {
                            if (!mesh->Initialize(direct3D)) {
                                Logger::Err("Failed to initialize 3D model!");
                                // Maybe quit here?
                                return;
                            }
                        }

                        TextureMesh* textureMesh = (TextureMesh*)mesh;

                        // Do some stuff to update the model specific constants that this specific shader uses
                        // Update model constant buffer
                        DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(mesh->rotation.x),
                            DirectX::XMConvertToRadians(mesh->rotation.y),
                            DirectX::XMConvertToRadians(mesh->rotation.z));
                        DirectX::XMMATRIX transMat = DirectX::XMMatrixTranslation(mesh->position.x, mesh->position.y, mesh->position.z);
                        DirectX::XMStoreFloat4x4(&textureShader->m_modelConstData.modelMatrix, DirectX::XMMatrixTranspose(rotMat * transMat));

                        UpdateBuffer(textureShader->m_modelConstBuffer, &textureShader->m_modelConstData, sizeof(textureShader->m_modelConstData));
                        direct3D->context->VSSetConstantBuffers(1, 1, &textureShader->m_modelConstBuffer);

                        // Render this specific model
                        UINT stride = sizeof(texture_shader_vertex_t);
                        UINT offset = 0;
                        direct3D->context->IASetVertexBuffers(0, 1, &mesh->vertexBuffer, &stride, &offset);
                        direct3D->context->IASetIndexBuffer(mesh->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
                        direct3D->context->PSSetShaderResources(0, 1, &textureMesh->m_texture); 
                        direct3D->context->DrawIndexed(mesh->indexCount, 0, 0);
                    }
                    break;
                }
                default: {
                    printf("Unknown shader type!");
                    break;
                }
            }
        }
    }
}
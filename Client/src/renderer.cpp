#include "renderer.h"
#include "mesh.h"
#include "logger.h"
#include "vertex.h"
#include "direct3d.h"
#include "util.h"
#include "camera.h"
#include "shader.h"
#include <DirectXMath.h>
#include "animation.h"

namespace PMG {
    Renderer::~Renderer() {
        for (Shader* shader : m_shaders) {
            delete shader;
        }

        for (auto mesh : meshes_) {
            delete mesh.second;
        }

        meshes_.clear();
    }

    void Renderer::Initialize(Direct3D* direct3D, int width, int height) {
        this->direct3D = direct3D;
        camera = new Camera();

        m_width = width;
        m_height = height;

        float hp = static_cast<float>(M_PI / 180.0);

        m_projMatrix = Physics::mat_t::Perspective((float)m_width / (float)m_height, camera->fov * hp, camera->nearClip, camera->farClip);

        // Where to set this?
        direct3D->context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // Set initial constant matrix values
        DirectX::XMStoreFloat4x4(&cameraMatrix, DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, DirectX::XMMatrixTranslation(
            camera->position.x, camera->position.y, camera->position.z))));

        // ------------ NEW ------------
        ColorShader* colorShader = new ColorShader();
        colorShader->Initialize(direct3D);
        m_shaders.push_back(colorShader);

        TextureShader* textureShader = new TextureShader();
        textureShader->Initialize(direct3D);
        m_shaders.push_back(textureShader);

        SkinnedTexturedShader* skinned_textured_shader = new SkinnedTexturedShader();
        skinned_textured_shader->Initialize(direct3D);
        m_shaders.push_back(skinned_textured_shader);

        // load resources?
        /*
        SkinnedTexturedMesh* mesh1 = (SkinnedTexturedMesh*) Mesh::LoadSkinnedTexturedMesh("models/chess_person.p3d", "models/chess_person.dds");
        mesh1->Initialize(direct3D);
        Animation* wave = Animation::LoadAnimation("models/chess_person_test.p3d_anim");
        mesh1->animations.emplace("wave", wave);
        meshes_.emplace("chess_person", mesh1);
        mesh1->CalculateMatrixPalette();

        Mesh* mesh2 = Mesh::LoadMesh("models/missile.p3d", "models/missile.dds");
        mesh2->Initialize(direct3D);
        meshes_.emplace("missile", mesh2);

        Mesh* mesh3 = Mesh::LoadMesh("models/tower.p3d", "models/tower.dds");
        mesh3->Initialize(direct3D);
        meshes_.emplace("tower", mesh3);
        */

        skinned_textured_shader_vertex_t verts[3]{
            {
                { 1.0f, 0.0f, 0.0f},
                { 1.0f, 0.0f, 0.0f},
                { 1.0f, 0.0f },
                { 0, 0, 0, 0 },
                { 1, 0, 0, 0 },
            },
            {
                { -1.0f, 0.0f, -1.0f},
                { -1.0f, 0.0f, 1.0f},
                { 1.0f, 0.0f },
                { 0, 0, 0, 0 },
                { 1, 0, 0, 0 },
            },
            {
                { -1.0f, 0.0f, 1.0f},
                { -1.0f, 0.0f, 1.0f},
                { 1.0f, 0.0f },
                { 0, 0, 0, 0 },
                { 1, 0, 0, 0 },
            },
        };
        // texture_shader_vertex_t* verts = new texture_shader_vertex_t[]{
        //     {
        //         { 1.0f, 0.0f, 0.0f},
        //         { 1.0f, 0.0f },
        //     },
        //     {
        //         { -1.0f, 0.0f, -1.0f},
        //         { 1.0f, 0.0f },
        //     },
        //     {
        //         { -1.0f, 0.0f, 1.0f},
        //         { 1.0f, 0.0f },
        //     },
        // };
        unsigned int* indices = new unsigned int[3]{ 0, 1, 2 };

        SkinnedTexturedMesh* test_mesh = (SkinnedTexturedMesh*) SkinnedTexturedMesh::LoadSkinnedTexturedMesh("models/test_person.p3d", "models/test_person.dds");
        test_mesh->Initialize(direct3D);
        // TextureMesh* test_mesh = new TextureMesh();
        // test_mesh->indexCount = 3;
        // test_mesh->vertexCount = 3;
        // test_mesh->vertices = verts;
        // test_mesh->indices = indices;
        // test_mesh->m_textureFileName = "models/test_person.dds";
        // test_mesh->Initialize(direct3D);

        Armature* arm = Armature::LoadArmature("models/test_person.p3d_skn");

        Animation* anim = new Animation();
        anim->bone_count = 1;
        anim->duration = 0.1;
        anim->frame_duration = 0.1;
        anim->frame_count = 1;
        anim->animation_tracks.resize(1);
        anim->animation_tracks[0].resize(1);

        anim = Animation::LoadAnimation("models/test_person_test_action.p3d_anim");
        test_mesh->animations.emplace("test_action", anim);

        test_mesh->armature = arm;
        test_mesh->animations.emplace("idle", anim);
        
        arm->ComputeGlobalInverseBindPoses();
        test_mesh->CalculateMatrixPalette();
        test_mesh->m_shaderType = ShaderType::SKINNED_TEXTURED;
        // test_mesh->m_shaderType = ShaderType::TEXTURE;
        meshes_.emplace("test", test_mesh);
    }

    void Renderer::SetDimensions(int width, int height) {
        m_width = width;
        m_height = height;

        float hp = static_cast<float>(M_PI / 180.0);
        m_projMatrix = Physics::mat_t::Perspective((float) m_width / (float) m_height, camera->fov * hp, camera->nearClip, camera->farClip);
    }

    void Renderer::UpdateCameraMatrix() {
        DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(camera->rotation.x),
                        DirectX::XMConvertToRadians(camera->rotation.y),
                        DirectX::XMConvertToRadians(camera->rotation.z));
        
        DirectX::XMMATRIX transMat =  DirectX::XMMatrixTranslation(camera->position.x, camera->position.y, camera->position.z);

        DirectX::XMStoreFloat4x4(&cameraMatrix, DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, rotMat * transMat)));
    }

    void Renderer::UpdateBuffer(ID3D11Buffer* buffer, const void* src, size_t size) {
        D3D11_MAPPED_SUBRESOURCE mappedResource = { 0 };
        direct3D->context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        memcpy(mappedResource.pData, src, size);
        direct3D->context->Unmap(buffer, 0);
    }

    void Renderer::RenderText(int x, int y, int w, int h, std::wstring text) {
        float color[3] = { 1.0, 1.0, 1.0 };
        RenderText(x, y, w, h, color, text);
    }

    void Renderer::RenderText(int x, int y, int w, int h, float color[3], std::wstring text) {
        //Set the Font Color
        D2D1_COLOR_F FontColor = D2D1::ColorF(color[0], color[1], color[2], 1.0f);

        ID2D1SolidColorBrush* brush;
        HRESULT hr = direct3D->renderTarget2D->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::Blue, 0.0f),
            &brush
        );

        if (FAILED(hr) || brush == nullptr) {
            return;
        }

        direct3D->format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        
        IDWriteTextLayout* textLayout;

        hr = direct3D->dWriteFactory->CreateTextLayout(
            text.c_str(),
            static_cast<UINT32>(wcslen(text.c_str())),
            direct3D->format,
            static_cast<float>(w),
            static_cast<float>(h),
            &textLayout
        );

        if (FAILED(hr) || textLayout == nullptr) {
            return;
        }

        //Set the brush color D2D will use to draw with
        brush->SetColor(FontColor);

        //Create the D2D Render Area
        D2D1_POINT_2F point = D2D1::Point2F(static_cast<float>(x), static_cast<float>(y));

        //Draw the Text
        direct3D->renderTarget2D->DrawTextLayout(
            point,
            textLayout,
            brush
        );

        brush->Release();
    }

    void Renderer::DrawRect(int x, int y, int w, int h, float color[3]) {
        D2D1_RECT_F rect{};
        rect.left = static_cast<float>(x);
        rect.top = static_cast<float>(y);
        rect.right = static_cast<float>(x + w);
        rect.bottom = static_cast<float>(y + h);

        //Set the Font Color
        D2D1_COLOR_F c = D2D1::ColorF(color[0], color[1], color[2]);


        ID2D1SolidColorBrush* brush;
        HRESULT hr = direct3D->renderTarget2D->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::Black, 1.0f),
            &brush
        );


        if (FAILED(hr) || brush == 0) {
            Logger::Err("Failed to create brush for rect");
            return;
        }

        brush->SetColor(c);

        direct3D->renderTarget2D->DrawRectangle(&rect, brush);

        brush->Release();
    }

    void Renderer::DrawShape(Physics::Vector2* points, int pointCount, float color[3]) {
        //Set the Font Color
        D2D1_COLOR_F c = D2D1::ColorF(color[0], color[1], color[2]);

        ID2D1SolidColorBrush* brush;
        HRESULT hr = direct3D->renderTarget2D->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::Black, 1.0f),
            &brush
        );


        if (FAILED(hr) || brush == 0) {
            Logger::Err("Failed to create brush for rect");
            return;
        }

        brush->SetColor(c);

        ID2D1PathGeometry* geometry;
        ID2D1GeometrySink* geometrySink = NULL;

        direct3D->d2d_factory_->CreatePathGeometry(&geometry);
        // Write to the path geometry using the geometry sink.
        geometry->Open(&geometrySink);
        geometrySink->BeginFigure({ static_cast<float>(points[0].x), static_cast<float>(points[0].y) }, D2D1_FIGURE_BEGIN_HOLLOW);

        for (int i = 1; i < pointCount; i++) {
            geometrySink->AddLine({ static_cast<float>(points[i].x), static_cast<float>(points[i].y) });
        }

        geometrySink->EndFigure(D2D1_FIGURE_END_CLOSED);
        hr = geometrySink->Close();

        if (FAILED(hr)) {
            Logger::Err("Failed to render shape");
        }

        direct3D->renderTarget2D->DrawGeometry(geometry, brush);

        geometrySink->Release();
        geometry->Release();
        brush->Release();
    };

    void Renderer::FillShape(Physics::Vector2* points, int pointCount, float color[3]) {
        if (pointCount < 2) {
            Logger::Err("Failed to draw shape: cannot draw shape from 1 point only");
            return;
        }

        //Set the Font Color
        D2D1_COLOR_F c = D2D1::ColorF(color[0], color[1], color[2]);

        ID2D1SolidColorBrush* brush;
        HRESULT hr = direct3D->renderTarget2D->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::Black, 1.0f),
            &brush
        );


        if (FAILED(hr) || brush == 0) {
            Logger::Err("Failed to create brush for rect");
            return;
        }

        brush->SetColor(c);

        ID2D1PathGeometry* geometry;
        ID2D1GeometrySink* geometrySink = NULL;

        direct3D->d2d_factory_->CreatePathGeometry(&geometry);
        // Write to the path geometry using the geometry sink.
        geometry->Open(&geometrySink);
        geometrySink->BeginFigure({ static_cast<float>(points[0].x), static_cast<float>(points[0].y) }, D2D1_FIGURE_BEGIN_FILLED);

        for (int i = 1; i < pointCount; i++) {
            geometrySink->AddLine({ static_cast<float>(points[i].x), static_cast<float>(points[i].y) });
        }

        geometrySink->EndFigure(D2D1_FIGURE_END_CLOSED);
        hr = geometrySink->Close();

        if (FAILED(hr)) {
            Logger::Err("Failed to render shape");
        }

        direct3D->renderTarget2D->FillGeometry(geometry, brush);

        geometrySink->Release();
        geometry->Release();
        brush->Release();
    };

    void Renderer::FillRect(int x, int y, int w, int h, float color[3]) {
        D2D1_RECT_F rect{};
        rect.left = static_cast<float>(x);
        rect.top = static_cast<float>(y);
        rect.right = static_cast<float>(x + w);
        rect.bottom = static_cast<float>(y + h);

        //Set the Font Color
        D2D1_COLOR_F c = D2D1::ColorF(color[0], color[1], color[2], 1.0f);


        ID2D1SolidColorBrush* brush;
        HRESULT hr = direct3D->renderTarget2D->CreateSolidColorBrush(c, &brush);


        if (FAILED(hr) || brush == 0) {
            Logger::Err("Failed to create brush for rect");
            return;
        }

        direct3D->renderTarget2D->FillRectangle(&rect, brush);

        brush->Release();
    }

    void Renderer::Render(GameObject* go) {
        Mesh* mesh = meshes_.find(go->mesh)->second;

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
                colorShader->m_frameConstData.projMatrix = m_projMatrix;
                UpdateBuffer(colorShader->m_frameConstBuffer, &colorShader->m_frameConstData, sizeof(colorShader->m_frameConstData));
                direct3D->context->VSSetConstantBuffers(0, 1, &colorShader->m_frameConstBuffer);

                // That uses this shader!
                if (mesh->m_shaderType != shader->m_type) {
                    continue;
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
                UINT stride = sizeof(color_shader_vertex_t);
                UINT offset = 0;
                direct3D->context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                direct3D->context->IASetVertexBuffers(0, 1, &mesh->vertexBuffer, &stride, &offset);
                direct3D->context->IASetIndexBuffer(mesh->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
                //direct3D->context->DrawIndexed(mesh->indexCount, 0, 0);
                break;
            }
            case ShaderType::TEXTURE: {
                TextureShader* textureShader = (TextureShader*)shader;
                // Do some stuff to update the frame constants for this shader

                // Update and set frame constant buffer
                textureShader->m_frameConstData.cameraMatrix = cameraMatrix;
                textureShader->m_frameConstData.projMatrix = m_projMatrix;
                UpdateBuffer(textureShader->m_frameConstBuffer, &textureShader->m_frameConstData, sizeof(textureShader->m_frameConstData));
                direct3D->context->VSSetConstantBuffers(0, 1, &textureShader->m_frameConstBuffer);
                direct3D->context->PSSetSamplers(0, 1, &textureShader->m_samplerState);
                // That uses this shader!
                if (mesh->m_shaderType != shader->m_type) {
                    continue;
                }

                TextureMesh* textureMesh = (TextureMesh*)mesh;

                // Do some stuff to update the model specific constants that this specific shader uses
                // Update model constant buffer
                DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(go->rotation.x),
                    DirectX::XMConvertToRadians(go->rotation.y),
                    DirectX::XMConvertToRadians(go->rotation.z));
                DirectX::XMMATRIX transMat = DirectX::XMMatrixTranslation(go->position.x, go->position.y, go->position.z);
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
                break;
            }
            case ShaderType::SKINNED_TEXTURED: {
                SkinnedTexturedShader* textureShader = (SkinnedTexturedShader*)shader;
                // Do some stuff to update the frame constants for this shader

                // Update and set frame constant buffer
                textureShader->m_frameConstData.cameraMatrix = cameraMatrix;
                textureShader->m_frameConstData.projMatrix = m_projMatrix;
                UpdateBuffer(textureShader->m_frameConstBuffer, &textureShader->m_frameConstData, sizeof(textureShader->m_frameConstData));
                direct3D->context->VSSetConstantBuffers(0, 1, &textureShader->m_frameConstBuffer);
                direct3D->context->PSSetSamplers(0, 1, &textureShader->m_samplerState);
                // That uses this shader!
                if (mesh->m_shaderType != shader->m_type) {
                    continue;
                }

                SkinnedTexturedMesh* textureMesh = (SkinnedTexturedMesh*)mesh;

                // Do some stuff to update the model specific constants that this specific shader uses
                // Update model constant buffer
                DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(go->rotation.x),
                    DirectX::XMConvertToRadians(go->rotation.y),
                    DirectX::XMConvertToRadians(go->rotation.z));
                DirectX::XMMATRIX transMat = DirectX::XMMatrixTranslation(go->position.x, go->position.y, go->position.z);
                DirectX::XMStoreFloat4x4(&textureShader->m_modelConstData.modelMatrix, DirectX::XMMatrixTranspose(rotMat * transMat));

                for (int i = 0; i < textureMesh->armature->bones.size(); i++) {
                    textureShader->m_modelConstData.animation_palette[i] = textureMesh->animation_palette[i];
                }

                UpdateBuffer(textureShader->m_modelConstBuffer, &textureShader->m_modelConstData, sizeof(textureShader->m_modelConstData));
                direct3D->context->VSSetConstantBuffers(1, 1, &textureShader->m_modelConstBuffer);

                // Render this specific model
                UINT stride = sizeof(skinned_textured_shader_vertex_t);
                UINT offset = 0;
                direct3D->context->IASetVertexBuffers(0, 1, &mesh->vertexBuffer, &stride, &offset);
                direct3D->context->IASetIndexBuffer(mesh->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
                direct3D->context->PSSetShaderResources(0, 1, &textureMesh->m_texture);
                direct3D->context->DrawIndexed(mesh->indexCount, 0, 0);
                break;
            }
            default: {
                printf("Unknown shader type!");
                break;
            }
            }
        }
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
                    colorShader->m_frameConstData.projMatrix = m_projMatrix;
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
                        UINT stride = sizeof(color_shader_vertex_t);
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
                    textureShader->m_frameConstData.projMatrix = m_projMatrix;
                    UpdateBuffer(textureShader->m_frameConstBuffer, &textureShader->m_frameConstData, sizeof(textureShader->m_frameConstData));
                    direct3D->context->VSSetConstantBuffers(0, 1, &textureShader->m_frameConstBuffer);
                    direct3D->context->PSSetSamplers(0, 1, &textureShader->m_samplerState);
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
                        //direct3D->context->DrawIndexed(mesh->indexCount, 0, 0);
                    }
                    break;
                }
                case ShaderType::SKINNED_TEXTURED: {
                    SkinnedTexturedShader* textureShader = (SkinnedTexturedShader*)shader;
                    // Do some stuff to update the frame constants for this shader

                    // Update and set frame constant buffer
                    textureShader->m_frameConstData.cameraMatrix = cameraMatrix;
                    textureShader->m_frameConstData.projMatrix = m_projMatrix;
                    UpdateBuffer(textureShader->m_frameConstBuffer, &textureShader->m_frameConstData, sizeof(textureShader->m_frameConstData));
                    direct3D->context->VSSetConstantBuffers(0, 1, &textureShader->m_frameConstBuffer);
                    direct3D->context->PSSetSamplers(0, 1, &textureShader->m_samplerState);
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

                        SkinnedTexturedMesh* textureMesh = (SkinnedTexturedMesh*)mesh;

                        // Do some stuff to update the model specific constants that this specific shader uses
                        // Update model constant buffer
                        DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(mesh->rotation.x),
                            DirectX::XMConvertToRadians(mesh->rotation.y),
                            DirectX::XMConvertToRadians(mesh->rotation.z));
                        DirectX::XMMATRIX transMat = DirectX::XMMatrixTranslation(mesh->position.x, mesh->position.y, mesh->position.z);
                        DirectX::XMStoreFloat4x4(&textureShader->m_modelConstData.modelMatrix, DirectX::XMMatrixTranspose(rotMat * transMat));

                        for (int i = 0; i < textureMesh->armature->bones.size(); i++) {
                            textureShader->m_modelConstData.animation_palette[i] = textureMesh->animation_palette[i];
                        }

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
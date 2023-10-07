#include "Renderer.h"
#include "Mesh.h"
#include "logger.h"
#include "Vertex.h"
#include "Direct3D.h"
#include "util.h"
#include "Camera.h"
#include "Shader.h"
#include <DirectXMath.h>
#include "Animation.h"
#include "ParticleShader.h"

namespace PMG {
    Renderer::~Renderer() {
        for (Shader* shader : m_shaders) {
            delete shader;
        }

        for (auto mesh_it : meshes_) {
            delete mesh_it.second;
        }
    }

    void Renderer::Initialize(Direct3D* direct3D, int width_, int height_) {
        this->direct3D = direct3D;
        camera = new Camera();

        m_width = width_;
        m_height = height_;

        float hp = static_cast<float>(M_PI / 180.0);

        DirectX::XMStoreFloat4x4(&m_projMatrix, DirectX::XMMatrixTranspose(DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(camera->fov), (float)m_width / (float)m_height, camera->nearClip, camera->farClip)));

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

        ParticleShader* particle_shader = new ParticleShader();
        particle_shader->Initialize(direct3D);
        m_shaders.push_back(particle_shader);

        // ------------ NEW ------------
        // SkinnedTexturedMesh* chess_person = SkinnedTexturedMesh::Load("models\\chess_person", direct3D);
        // meshes_.emplace("chess_person", chess_person);
        // 
        // TextureMesh* tower = TextureMesh::Load("models/tower", direct3D);
        // meshes_.emplace("tower", tower);
        // 
        // TextureMesh* missile = TextureMesh::Load("models/missile", direct3D);
        // meshes_.emplace("missile", missile);
        // 
        // TextureMesh* minion = TextureMesh::Load("models\\cube_minion", direct3D);
        // meshes_.emplace("minion", minion);
    }

    void Renderer::SetDimensions(int width_, int height_) {
        m_width = width_;
        m_height = height_;

        float hp = static_cast<float>(M_PI / 180.0);
        DirectX::XMStoreFloat4x4(&m_projMatrix, DirectX::XMMatrixTranspose(DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(camera->fov), (float) m_width / (float) m_height, camera->nearClip, camera->farClip)));
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

    void Renderer::RenderText(int x, int y, int w, int h, std::string text) {
        float color[3] = { 1.0, 1.0, 1.0 };
         RenderText(x, y, w, h, color, text);
    }

    void Renderer::RenderText(int x, int y, int w, int h, float color[3], std::string text) {
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
        
        std::wstring wstr;
        int convertResult = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), (int)strlen(text.c_str()), NULL, 0);
        wstr.resize(convertResult);
        convertResult = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), (int)strlen(text.c_str()), &wstr[0], (int)wstr.size());

        hr = direct3D->dWriteFactory->CreateTextLayout(
            wstr.c_str(),
            wstr.length(),
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
        textLayout->Release();
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

    void Renderer::BindShader(ShaderType type) {
        Shader* shader = nullptr;

        for (Shader* s : m_shaders) {
            if (s->m_type == type) {
                shader = s;
                break;
            }
        }

        if (shader == nullptr) {
            Logger::Err("Failed to find shader");
            return;
        }

        direct3D->context->VSSetShader(shader->m_vertexShader, 0, 0);
        direct3D->context->PSSetShader(shader->m_pixelShader, 0, 0);
        direct3D->context->IASetInputLayout(shader->m_inputLayout);
    }

    template<>
    void Renderer::UpdateShaderConst(texture_shader_frame_const_t const_data) {
        TextureShader* textureShader = nullptr;

        for (Shader* s : m_shaders) {
            if (s->m_type == ShaderType::TEXTURE) {
                textureShader = (TextureShader*) s;
                break;
            }
        }

        if (textureShader == nullptr) {
            Logger::Err("Failed to find shader");
            return;
        }

        UpdateBuffer(textureShader->m_frameConstBuffer, &const_data, sizeof(textureShader->m_frameConstData));
        direct3D->context->VSSetConstantBuffers(0, 1, &textureShader->m_frameConstBuffer);
        direct3D->context->PSSetSamplers(0, 1, &textureShader->m_samplerState);
    }

    template<>
    void Renderer::UpdateShaderConst(texture_shader_model_const_t const_data) {
        TextureShader* textureShader = nullptr;

        for (Shader* s : m_shaders) {
            if (s->m_type == ShaderType::TEXTURE) {
                textureShader = (TextureShader*)s;
                break;
            }
        }

        if (textureShader == nullptr) {
            Logger::Err("Failed to find shader");
            return;
        }

        UpdateBuffer(textureShader->m_modelConstBuffer, &const_data, sizeof(textureShader->m_modelConstData));
        direct3D->context->VSSetConstantBuffers(1, 1, &textureShader->m_modelConstBuffer);
    }

    template<>
    void Renderer::UpdateShaderConst(particle_shader_frame_const_t const_data) {
        ParticleShader* shader = nullptr;

        for (Shader* s : m_shaders) {
            if (s->m_type == ShaderType::PARTICLE) {
                shader = (ParticleShader*)s;
                break;
            }
        }

        if (shader == nullptr) {
            Logger::Err("Failed to find shader");
            return;
        }

        UpdateBuffer(shader->m_frameConstBuffer, &const_data, sizeof(shader->m_frameConstData));
        direct3D->context->VSSetConstantBuffers(0, 1, &shader->m_frameConstBuffer);
    }

    template<>
    void Renderer::UpdateShaderConst(particle_shader_model_const_t const_data) {
        ParticleShader* shader = nullptr;

        for (Shader* s : m_shaders) {
            if (s->m_type == ShaderType::PARTICLE) {
                shader = (ParticleShader*)s;
                break;
            }
        }

        if (shader == nullptr) {
            Logger::Err("Failed to find shader");
            return;
        }

        UpdateBuffer(shader->m_modelConstBuffer, &const_data, sizeof(shader->m_modelConstData));
        direct3D->context->VSSetConstantBuffers(1, 1, &shader->m_modelConstBuffer);
    }

    template<>
    void Renderer::UpdateShaderConst(color_shader_frame_const_t const_data) {
        ColorShader* shader = nullptr;

        for (Shader* s : m_shaders) {
            if (s->m_type == ShaderType::COLOR) {
                shader = (ColorShader*)s;
                break;
            }
        }

        if (shader == nullptr) {
            Logger::Err("Failed to find shader");
            return;
        }

        UpdateBuffer(shader->m_frameConstBuffer, &const_data, sizeof(shader->m_frameConstData));
        direct3D->context->VSSetConstantBuffers(0, 1, &shader->m_frameConstBuffer);
    }

    template<>
    void Renderer::UpdateShaderConst(color_shader_model_const_t const_data) {
        ColorShader* textureShader = nullptr;

        for (Shader* s : m_shaders) {
            if (s->m_type == ShaderType::COLOR) {
                textureShader = (ColorShader*)s;
                break;
            }
        }

        if (textureShader == nullptr) {
            Logger::Err("Failed to find shader");
            return;
        }

        textureShader->m_modelConstData.modelMatrix = const_data.modelMatrix;

        UpdateBuffer(textureShader->m_modelConstBuffer, &textureShader->m_modelConstData, sizeof(textureShader->m_modelConstData));
        direct3D->context->VSSetConstantBuffers(1, 1, &textureShader->m_modelConstBuffer);
    }

    void Renderer::SetVertexBuffer(ID3D11Buffer* buffer, UINT stride, UINT offset) {
        direct3D->context->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);
    }

    void Renderer::SetVertexBuffers(ID3D11Buffer** buffers, UINT* stride, UINT* offset) {
        direct3D->context->IASetVertexBuffers(0, 2, buffers, stride, offset);
    }

    void Renderer::SetIndexBuffer(ID3D11Buffer* buffer) {
        direct3D->context->IASetIndexBuffer(buffer, DXGI_FORMAT_R32_UINT, 0);
    }

    void Renderer::SetShaderResource(UINT start_slot, UINT num_views, ID3D11ShaderResourceView* resource) {
        direct3D->context->PSSetShaderResources(0, 1, &resource);
    }

    void Renderer::DrawIndexed(int count) {
        direct3D->context->DrawIndexed(count, 0, 0);
    }

    void Renderer::DrawInstanced(int vertex_count, int instance_count) {
        direct3D->context->DrawInstanced(vertex_count, instance_count, 0, 0);
    }

    void Renderer::EnableAlphaBlending() {
        direct3D->EnableAlphaBlending();
        direct3D->EnableDepthStencilState();
    }

    void Renderer::DisableAlphaBlending() {
        direct3D->DisableAlphaBlending();
        direct3D->DisableDepthStencilState();
    }
}
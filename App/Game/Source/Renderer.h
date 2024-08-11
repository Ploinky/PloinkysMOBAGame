#pragma once

#include <d3d11.h>
#include <d2d1_1.h>
#include <DirectXMath.h>
#include <vector>
#include <string>
#include "common/pmg_physics.h"
#include <map>
#include <Common/PMG_Common.h>
#include <Model.h>
#include <PloinkysJSONLibrary.h>
#include <GLBFileLoader.h>
#include <GLBShader.h>
#include <ParticleShader.h>
#include <ParticleEmitter.h>
#include <ParticleSystem.h>
#include <Camera.h>
#include <Mesh.h>
#include <Armature.h>
#include <Common/PMG_Common.h>
#include "Direct3D.h"
#include "Camera.h"
#include <DirectXMath.h>
#include "Animation.h"
#include "ParticleShader.h"
#include "GameObject.h"
#include "wincodec.h"

namespace PMG {
    class Renderable;
    class GameObject;

    enum class TextureId {
        FONT
    };

    enum class BitmapId {
        BUTTON_MENU,
    };

    class CRenderer {
        public:
            ~CRenderer();
            void Initialize(HWND hWindowHandle, bool bFullScreen, AssetManager* assetManager, int width_, int height_);
			void LoadResources(AssetManager* pAssetManager);
            void SetDimensions(int width_, int height_);
            void UpdateCameraMatrix();

            Camera m_camera;
            Direct3D m_d3d;
            DirectX::XMFLOAT4X4 m_projMatrix;
            DirectX::XMFLOAT4X4 cameraMatrix;

            void RenderText(int x, int y, int w, int h, float color[3], std::string text);
            void RenderText(int x, int y, int w, int h, std::string text);
            void DrawRect(int x, int y, int w, int h, float color[3]);
            void DrawShape(Physics::Vector2* points, int pointCount, float color[3]);
            void FillShape(Physics::Vector2* points, int pointCount, float color[3]);
            void FillRect(int x, int y, int w, int h, float color[3]);
            void DrawImage(float x, float y, float w, float h, BitmapId id);
            void UpdateBuffer(ID3D11Buffer* buffer, const void* src, size_t size);

            template<typename T>
            void UpdateShaderConst(T const_data);
            
            void Draw(GameObject* gameObject);
			void Draw(Model* modelNode);
			void RenderParticle(ParticleEmitter* emitter);

			void DrawMap();

			void ClearScreen();
			void Present();
			void SetFullscreen(bool bFullscreen);

			// Renders a semi-transparent gray cover over the specified area, for example for cooldowns.
			void RenderPartialCover(float fX, float fY, float fWidth, float fHeight, float fCoverage);

        private:
            int m_width;
            int m_height;

            std::map<std::string, Renderable*> meshes_;
            std::map<std::string, ID3D11ShaderResourceView*> textures_;
            std::map<BitmapId, ID2D1Bitmap*> bitmaps_;
			std::map<std::string, Model*> models_;

			void CreateShaderResourceViewFromPNG(std::vector<uint8_t> imageData, ID3D11ShaderResourceView** shaderResourceView);
            ID2D1Bitmap* CreateBitmapFromData(std::vector<uint8_t> data);
            ID2D1Bitmap* CreateBitmapFromFile(const wchar_t* fileName);

			void LoadGLBModel(std::string name, std::string file, AssetManager* assetManager);
			ModelNode* LoadNode(GLBNode* glbNode);
			Mesh* LoadMesh(GLBModelMesh* glbMesh);
			
            void EnableAlphaBlending();
            void DisableAlphaBlending();
			
			ParticleShader* particleShader_;
			GLBShader* glbShader_;
    };
}
#pragma once

#include <vector>
#include <string>
#include "common/pmg_physics.h"
#include <map>
#include <common/PMG_Common.h>
#include <Model.h>
#include <common/PloinkysJSONLibrary.h>
#include <GLBFileLoader.h>
#include <Camera.h>
#include <Mesh.h>
#include <Armature.h>
#include "core/graphics/graphics-engine.h"
#include "Animation.h"
#include "GameObject.h"
#include <common/navigation.h>
#include "client-asset-manager.h"
#include "core/rendering/render-command.h"
#include "common/game/game-state.h"

class GameObject;
class ParticleEffect;
class ParticleEmitter;

typedef struct FlatUnlitShaderVertex_t {
	float position[3];
	float color[4];
} FlatUnlitShaderVertex_t;

typedef struct SkinnnedMeshShaderVertex_t {
	float position[3];
	float normal[3];
	float texCoord[2];
	uint8_t joints[4];
	float weights[4];
} SkinnnedMeshShaderVertex_t;

typedef struct ParticleShaderVertex_t {
	float position[3];
	float tex_coord[2];
} ParticleShaderVertex_t;

typedef struct ParticleShaderVertexInstance_t {
	float instance_position[3];
} ParticleShaderVertexInstance_t;

class CRenderer {
public:
    CRenderer(IGraphicsEngine* pGraphicsEngine);
    ~CRenderer();
    void Initialize(bool bFullScreen, CClientAssetManager* assetManager, int width_, int height_);
    void LoadResources(CClientAssetManager* pAssetManager);
    void SetDimensions(int width_, int height_);
    void UpdateCameraMatrix();

    Camera m_camera;
    mat m_projMatrix;
    mat cameraMatrix;

    void RenderText(int x, int y, int w, int h, float color[3], std::string text);
    void RenderText(int x, int y, int w, int h, std::string text);
    void DrawRect(int x, int y, int w, int h, float color[3]);
    void DrawShape(Vector2* points, int pointCount, float color[3]);
    void FillShape(Vector2* points, int pointCount, float color[3]);
    void FillRect(int x, int y, int w, int h, float color[3]);
    void DrawImage(float x, float y, float w, float h, HBitmap hBitmap);

    void DrawMap();

    void Render(CGameState* pGameState);

    void ClearScreen();
    void Present();
    void SetFullscreen(bool bFullscreen);

    // Renders a semi-transparent gray cover over the specified area, for example for cooldowns.
    void RenderPartialCover(float fX, float fY, float fWidth, float fHeight, float fCoverage);


    void RenderChat(std::vector<std::string> vecMsgs);

#ifdef NAV_DEBUG
    // Renders the navigation cell grid over the map
    void RenderNavGrid(NavigationCellGrid* grid);
#endif
    void QueueParticle(ParticleEmitter* pParticleEmitter);
    void Submit(RenderCommand_t command);

private:
    IGraphicsEngine* m_pGraphicsEngine;
    int m_width;
    int m_height;

    std::map<std::string, Model*> models_;

    bool InitParticleEmitter(ParticleEmitter* pEmitter);
    bool InitParticleEffect(ParticleEffect* pSystem);

    HShaderProgram m_hGlbShaderProgram;
    HShaderProgram m_hParticleShaderProgram;
    HShaderProgram m_hFlatUnlitShaderProgram;

    BufferHandle_t m_hFrameConstBuffer;
    BufferHandle_t m_hModelConstBuffer;
    BufferHandle_t m_hSkinnedModelConstBuffer;
    BufferHandle_t m_hBillboardFrameConstBuffer;

    BufferHandle_t m_hParticleVertexBuffer;
        
    #ifdef NAV_DEBUG
    // --- navigation grid rendering ---
    BufferHandle_t m_pNavGridVertexBuffer;
    BufferHandle_t m_pNavGridIndexBuffer;
    #endif
    
    CClientAssetManager* m_pAssetManager;

    std::vector<RenderCommand_t> m_vecCommands;

    void UploadModelToGPU(ModelAsset_t& modelAsset);
    Mesh* LoadMesh(GLBModelMesh glbMesh);
    ModelNode* LoadNode(GLBNode glbNode);

    void Draw(RenderCommand_t cmd);
    void Draw(Model* modelNode);
    void RenderParticle(ParticleEmitter* emitter);
};

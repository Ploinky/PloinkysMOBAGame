#include "Renderer.h"

#include "common/PloinkysJSONLibrary.h"
#include "client-asset-manager.h"
#include "ParticleEmitter.h"
#include "ParticleSystem.h"
#include "game/components/components.h"

CRenderer::~CRenderer() {
	for (auto model_it : models_) {
		delete model_it.second;
	}

#ifdef _DEBUG
	((ID3D11Buffer*) m_pNavGridIndexBuffer.ptr)->Release();
	((ID3D11Buffer*) m_pNavGridVertexBuffer.ptr)->Release();
#endif
}

void CRenderer::Initialize(HWND hWindowHandle, bool bFullScreen, CClientAssetManager* assetManager, int width_, int height_) {
	m_pGraphicsEngine = IGraphicsEngine::Create(hWindowHandle, width_, height_); // TODO bFullScreen);
	
    m_width = width_;
    m_height = height_;

    float hp = static_cast<float>(M_PI / 180.0);

    DirectX::XMStoreFloat4x4(&m_projMatrix, DirectX::XMMatrixTranspose(DirectX::XMMatrixPerspectiveFovRH(DirectX::XMConvertToRadians(m_camera.fov), (float)m_width / (float)m_height, m_camera.nearClip, m_camera.farClip)));

    // Set initial constant matrix values
    DirectX::XMStoreFloat4x4(&cameraMatrix, DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, DirectX::XMMatrixTranslation(
        m_camera.position.x, m_camera.position.y, m_camera.position.z))));

	m_pAssetManager = assetManager;

	m_hFrameConstBuffer = m_pGraphicsEngine->CreateConstantBuffer(sizeof(FrameConstants_t), nullptr);
	m_hModelConstBuffer = m_pGraphicsEngine->CreateConstantBuffer(sizeof(ModelConstants_t), nullptr);
	m_hSkinnedModelConstBuffer = m_pGraphicsEngine->CreateConstantBuffer(sizeof(SkinnedModelConstants_t), nullptr);
	m_hBillboardFrameConstBuffer = m_pGraphicsEngine->CreateConstantBuffer(sizeof(BillboardFrameConstants_t), nullptr);
}

void CRenderer::LoadResources(CClientAssetManager* pAssetManager) {
    // ------------ NEW ------------
	m_hGlbShaderProgram = m_pGraphicsEngine->LoadShaderProgram("glb", EVertexFormat::SKINNED_MESH, pAssetManager);
	m_hParticleShaderProgram = m_pGraphicsEngine->LoadShaderProgram("particle", EVertexFormat::PARTICLE, pAssetManager);
	m_hFlatUnlitShaderProgram = m_pGraphicsEngine->LoadShaderProgram("flat_unlit", EVertexFormat::STATIC_MESH, pAssetManager);

    // ------------ TEXTURES ------------
	// TODO do we need this somewhere lelse
	// bitmaps_.emplace("MissingTexture", CreateBitmapFromData(pAssetManager->LoadFile("Generic/missing_texture.bmp")));

	// ------------ IMAGES ------------
	m_pAssetManager->LoadTexture("UI/MoveTo/move_to.png");
	m_pAssetManager->LoadTexture("Persons/ChessPerson/blast_area.png");
	m_pAssetManager->LoadTexture("Persons/ChessPerson/particle.png");
	m_pAssetManager->LoadTexture("characters/stormcaller/abilities/thunderstrike.png");

#ifdef _DEBUG
	FlatUnlitShaderVertex_t vertices[4] = {
		{{0, 10, 0}, {1, 0, 0, 1}},
		{{50, 10, 0}, {1, 0, 0, 1}},
		{{50, 10, -50}, {1, 0, 0, 1}},
		{{0, 10, -50}, {1, 0, 0, 1}},
	};

	unsigned int indices[6] = { 0, 1, 2, 0, 2, 3 };

	m_pNavGridVertexBuffer = m_pGraphicsEngine->CreateVertexBuffer(vertices, sizeof(FlatUnlitShaderVertex_t) * 4, 4);
	m_pNavGridIndexBuffer = m_pGraphicsEngine->CreateIndexBuffer(indices, 6);
#endif
}

void CRenderer::SetDimensions(int width_, int height_) {
	m_pGraphicsEngine->SetWindowDimensions(width_, height_);

    m_width = width_;
    m_height = height_;

    float hp = static_cast<float>(M_PI / 180.0);
    DirectX::XMStoreFloat4x4(&m_projMatrix, DirectX::XMMatrixTranspose(DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(m_camera.fov), (float)m_width / (float)m_height, m_camera.nearClip, m_camera.farClip)));
}

void CRenderer::UpdateCameraMatrix() {
    DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(m_camera.rotation.x),
        DirectX::XMConvertToRadians(m_camera.rotation.y),
        DirectX::XMConvertToRadians(m_camera.rotation.z));

    DirectX::XMMATRIX transMat = DirectX::XMMatrixTranslation(m_camera.position.x, m_camera.position.y, m_camera.position.z);

    DirectX::XMStoreFloat4x4(&cameraMatrix, DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, rotMat * transMat)));
}

void CRenderer::RenderText(int x, int y, int w, int h, std::string text) {
    float color[3] = { 1.0, 1.0, 1.0 };
    RenderText(x, y, w, h, color, text);
}

void CRenderer::RenderText(int x, int y, int w, int h, float color[3], std::string text) {
	ICanvas2D* pCanvas = m_pGraphicsEngine->GetCanvas2D();
	pCanvas->RenderText(x, y, w, h, color, text);
}

void CRenderer::DrawRect(int x, int y, int w, int h, float color[3]) {
	ICanvas2D* pCanvas = m_pGraphicsEngine->GetCanvas2D();
	pCanvas->DrawRect(x, y, w, h, color);
}

void CRenderer::DrawShape(Vector2* points, int pointCount, float color[3]) {
	ICanvas2D* pCanvas = m_pGraphicsEngine->GetCanvas2D();
	pCanvas->DrawShape(points, pointCount, color);
};

void CRenderer::FillShape(Vector2* points, int pointCount, float color[3]) {
	ICanvas2D* pCanvas = m_pGraphicsEngine->GetCanvas2D();
	pCanvas->FillShape(points, pointCount, color);
};

void CRenderer::FillRect(int x, int y, int w, int h, float color[3]) {
	ICanvas2D* pCanvas = m_pGraphicsEngine->GetCanvas2D();
	pCanvas->FillRect(x, y, w, h, color);
}

void CRenderer::DrawImage(float x, float y, float w, float h, HBitmap hBitmap) {
	if(hBitmap == INVALID_ASSET_HANDLE) {
		Logger::Err("Attempting to draw invalid image");
		return;
	}

	ICanvas2D* pCanvas = m_pGraphicsEngine->GetCanvas2D();
	BitmapAsset_t& bmp = m_pAssetManager->GetBitmapImage(hBitmap);
	pCanvas->DrawImage(x, y, w, h, bmp);
}

void CRenderer::Draw(RenderCommand_t cmd) {
	ModelAsset_t& modelAsset = m_pAssetManager->GetModel(cmd.hModel);
	Model* model = modelAsset.pModel;


	m_pGraphicsEngine->BindShaderProgram(m_hGlbShaderProgram);

	FrameConstants_t frameConstants {};
	frameConstants.cameraMatrix = cameraMatrix;
	frameConstants.projMatrix = m_projMatrix;
	m_pGraphicsEngine->UpdateBuffer(m_hFrameConstBuffer, &frameConstants, sizeof(FrameConstants_t));
	m_pGraphicsEngine->BindVertexShaderConstantBuffer(0, m_hFrameConstBuffer);

	ModelConstants_t modelConstants {};
	DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationRollPitchYaw(
		DirectX::XMConvertToRadians(cmd.vec3Rotation.x),
		DirectX::XMConvertToRadians(cmd.vec3Rotation.y),
		DirectX::XMConvertToRadians(cmd.vec3Rotation.z));
	DirectX::XMMATRIX transMat = DirectX::XMMatrixTranslation(cmd.vec3Position.x, cmd.vec3Position.y, cmd.vec3Position.z);
	DirectX::XMStoreFloat4x4(&modelConstants.modelMatrix, DirectX::XMMatrixTranspose(rotMat * transMat));
	m_pGraphicsEngine->UpdateBuffer(m_hModelConstBuffer, &modelConstants, sizeof(ModelConstants_t));
	m_pGraphicsEngine->BindVertexShaderConstantBuffer(1, m_hModelConstBuffer);

	SkinnedModelConstants_t skinnedModelConstants {};
	for(int i = 0; i < 256; i++) {
		skinnedModelConstants.boneTransforms[i] = cmd.vecBones[i];
	}
	
	m_pGraphicsEngine->UpdateBuffer(m_hSkinnedModelConstBuffer, &skinnedModelConstants, sizeof(SkinnedModelConstants_t));
	m_pGraphicsEngine->BindVertexShaderConstantBuffer(2, m_hSkinnedModelConstBuffer);

	if(cmd.hTexture != INVALID_ASSET_HANDLE) {
		TextureAsset_t& textureAsset = m_pAssetManager->GetTexture(cmd.hTexture);
		if(textureAsset.pTexture == nullptr) {
			m_pGraphicsEngine->LoadTextureDataToGPU(textureAsset);
		}
		m_pGraphicsEngine->BindTexture(0, textureAsset);
	}

	m_pGraphicsEngine->SetVertexBuffer(0, cmd.hVertexBuffer, sizeof(SkinnnedMeshShaderVertex_t), 0); // TODO glb_shader_vertex_t ??
	m_pGraphicsEngine->SetIndexBuffer(cmd.hIndexBuffer);
	m_pGraphicsEngine->DrawIndexed(cmd.uIndexCount);
}

void CRenderer::Draw(Model* model) {
	for(const auto& modelNode : model->Nodes) {
		Mesh* mesh = model->Meshes.at(modelNode.second->Mesh);
		if(mesh == nullptr) {
			continue;
		}

		if(mesh->MaterialIndex != -1 && model->Materials.size() > mesh->MaterialIndex) {
			TextureAsset_t& textureAsset = m_pAssetManager->GetTexture(model->Materials.at(mesh->MaterialIndex)->hTexture);
			if(textureAsset.pTexture == nullptr) {
 				m_pGraphicsEngine->LoadTextureDataToGPU(textureAsset);
			}
			m_pGraphicsEngine->BindTexture(0, textureAsset);
		}

		m_pGraphicsEngine->SetVertexBuffer(0, mesh->VertexBuffer, sizeof(SkinnnedMeshShaderVertex_t), 0);
		m_pGraphicsEngine->SetIndexBuffer(mesh->IndexBuffer);
		m_pGraphicsEngine->DrawIndexed(mesh->IndexCount);
	}
}

void CRenderer::RenderParticle(ParticleEmitter* emitter) {
	m_pGraphicsEngine->BindShaderProgram(m_hParticleShaderProgram);

	float rotY = atan2(emitter->position.x - m_camera.position.x, emitter->position.z - m_camera.position.z);
	float rotX = -atan2(emitter->position.y -m_camera.position.y, m_camera.position.z- emitter->position.z);
	float rotZ = 0; // no camera roll

	if (emitter->static_angle) {
		rotX = DirectX::XMConvertToRadians(emitter->particle_angle.x);
		rotY = DirectX::XMConvertToRadians(emitter->particle_angle.y);
		rotZ = DirectX::XMConvertToRadians(emitter->particle_angle.z);
	}

	FrameConstants_t data{};
	data.cameraMatrix = cameraMatrix;
	data.projMatrix = m_projMatrix;
	BillboardFrameConstants_t bbData {};
	DirectX::XMStoreFloat4x4(&bbData.billboardMatrix, DirectX::XMMatrixTranspose(DirectX::XMMatrixRotationRollPitchYaw(rotX, rotY, rotZ)));

	m_pGraphicsEngine->UpdateBuffer(m_hFrameConstBuffer, &data, sizeof(FrameConstants_t));
	m_pGraphicsEngine->UpdateBuffer(m_hBillboardFrameConstBuffer, &bbData, sizeof(BillboardFrameConstants_t));

	ModelConstants_t modelData {};
	DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(emitter->rotation.x),
		DirectX::XMConvertToRadians(emitter->rotation.y),
		DirectX::XMConvertToRadians(emitter->rotation.z));
	DirectX::XMMATRIX transMat = DirectX::XMMatrixTranslation(emitter->position.x, emitter->position.y, emitter->position.z);
	DirectX::XMMATRIX scaleMat = DirectX::XMMatrixScaling(emitter->particle_scale.x, emitter->particle_scale.y, emitter->particle_scale.z);
	DirectX::XMStoreFloat4x4(&modelData.modelMatrix, DirectX::XMMatrixTranspose(scaleMat * rotMat * transMat));

	m_pGraphicsEngine->UpdateBuffer(m_hModelConstBuffer, &modelData, sizeof(ModelConstants_t));

	std::vector<ParticleShaderVertexInstance_t> instances;

	std::sort(emitter->particles.begin(), emitter->particles.end(), [this, emitter](Particle& a, Particle& b) {
		Vector3 aVec = a.position + emitter->position;
		aVec.y = 0;
		Vector3 bVec = b.position + emitter->position;
		bVec.y = 0;
		return (m_camera.position - a.position).Length() > (m_camera.position - b.position).Length();
	});

	for (const Particle& particle : emitter->particles) {
		ParticleShaderVertexInstance_t p;
		p.instance_position[0] = particle.position.x;
		p.instance_position[1] = particle.position.y;
		p.instance_position[2] = particle.position.z;
		instances.push_back(p);
	}

	if(!emitter->instance_buffer_) {
		emitter->instance_buffer_ = m_pGraphicsEngine->CreateInstanceBuffer(instances.data(), instances.size(), sizeof(ParticleShaderVertexInstance_t) * instances.size());
	} else {
		m_pGraphicsEngine->UpdateBuffer(emitter->instance_buffer_, (void*) instances.data(), sizeof(ParticleShaderVertexInstance_t) * instances.size());
	}

	if(!emitter->vertex_buffer_) {
		ParticleShaderVertex_t vertices[6]{
			{ {-1, 1, 0}, {0, 0} },
			{ {1, 1, 0}, {1, 0} },
			{ {1, -1, 0}, {1, 1} },
			{ {-1, 1, 0}, {0, 0} },
			{ {1, -1, 0}, {1, 1} },
			{ {-1, -1, 0}, {0, 1} },
		};
		emitter->vertex_buffer_ = m_pGraphicsEngine->CreateVertexBuffer(vertices, sizeof(ParticleShaderVertex_t) * 6, 6);
	}


 	m_pGraphicsEngine->SetVertexBuffer(0, emitter->vertex_buffer_, sizeof(ParticleShaderVertex_t), 0);
	m_pGraphicsEngine->SetVertexBuffer(1, emitter->instance_buffer_, sizeof(ParticleShaderVertexInstance_t), 0);

	HTexture hTexture = m_pAssetManager->LoadTexture(emitter->texture_name_);
	TextureAsset_t& textureAsset = m_pAssetManager->GetTexture(hTexture);

	if(textureAsset.pTexture == nullptr) {
		m_pGraphicsEngine->LoadTextureDataToGPU(textureAsset);
	}

	m_pGraphicsEngine->BindTexture(0, textureAsset);

	m_pGraphicsEngine->BindVertexShaderConstantBuffer(0, m_hFrameConstBuffer);
	m_pGraphicsEngine->BindVertexShaderConstantBuffer(1, m_hModelConstBuffer);
	m_pGraphicsEngine->BindVertexShaderConstantBuffer(2, m_hBillboardFrameConstBuffer);
	
	m_pGraphicsEngine->EnableAlphaBlending();
    m_pGraphicsEngine->EnableDepthStencilState();
	m_pGraphicsEngine->DrawInstanced(6, instances.size());
	m_pGraphicsEngine->DisableAlphaBlending();
    m_pGraphicsEngine->DisableDepthStencilState();
}

void CRenderer::DrawMap() {
	m_pGraphicsEngine->BindShaderProgram(m_hGlbShaderProgram);

	FrameConstants_t data{};
	data.cameraMatrix = cameraMatrix;
	data.projMatrix = m_projMatrix;
	m_pGraphicsEngine->UpdateBuffer(m_hFrameConstBuffer, &data, sizeof(FrameConstants_t));
	m_pGraphicsEngine->BindVertexShaderConstantBuffer(0, m_hFrameConstBuffer);

	ModelConstants_t modelData{};
	DirectX::XMMATRIX rotMat = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX transMat = DirectX::XMMatrixTranslation(0, 0, 0);
	DirectX::XMStoreFloat4x4(&modelData.modelMatrix, DirectX::XMMatrixTranspose(rotMat * transMat));
	m_pGraphicsEngine->UpdateBuffer(m_hModelConstBuffer, &modelData, sizeof(ModelConstants_t));
	m_pGraphicsEngine->BindVertexShaderConstantBuffer(1, m_hModelConstBuffer);

	SkinnedModelConstants_t skinConst{};
	m_pGraphicsEngine->UpdateBuffer(m_hSkinnedModelConstBuffer, &skinConst, sizeof(SkinnedModelConstants_t));
	m_pGraphicsEngine->BindVertexShaderConstantBuffer(2, m_hSkinnedModelConstBuffer);

	HModel hModel = m_pAssetManager->LoadModel("map1");
	ModelAsset_t& modelAsset = m_pAssetManager->GetModel(hModel);
	
	if(modelAsset.pModel == nullptr) {
		modelAsset.pModel = new Model();
		for(const auto& glbNode : modelAsset.pGlbModel->Nodes) {
			ModelNode* modelNode = LoadNode(glbNode.second);
			modelAsset.pModel->Nodes.emplace(glbNode.first, modelNode);
		}

		for(const auto& glbMesh : modelAsset.pGlbModel->Meshes) {
			Mesh* mesh = LoadMesh(glbMesh.second);
			modelAsset.pModel->Meshes.emplace(glbMesh.first, mesh);
		}

		for(const auto& m : modelAsset.pGlbModel->Materials) {
			Material* material = new Material();
			material->hTexture = m_pAssetManager->LoadTextureFromData(m.second->TextureData);
			modelAsset.pModel->Materials.emplace(m.first, material);
		}
							
				for(const auto& skin : modelAsset.pGlbModel->Skins) {
					Armature* armature = new Armature();
					const auto& glbSkin = skin.second;

					for(int i = 0; i < glbSkin->Joints.size(); i++) {
						const auto& joint = glbSkin->Joints[i];
						Bone bone = Bone();
						bone.Index = joint;
						armature->bones.push_back(bone);
					}

					for(int i = 0; i < glbSkin->Joints.size(); i++) {
						for(auto childIndex : modelAsset.pGlbModel->Nodes[glbSkin->Joints[i]]->Children) {
							for(int j = 0; j < armature->bones.size(); j++) {
								if(armature->bones[j].Index == childIndex) {
									armature->bones[j].parent_index = glbSkin->Joints[i];
								}
							}
						}
					}

					armature->global_inverse_bind_poses = glbSkin->InverseBindMatrices;

					modelAsset.pModel->Skins.emplace(skin.first, armature);
				}

				for(const auto& glbAnimationEntry : modelAsset.pGlbModel->Animations) {
					GLBAnimation* glbAnimation = glbAnimationEntry.second;
					Animation* animation = new Animation();
					modelAsset.pModel->Animations.emplace(glbAnimation->Name, animation);

					animation->duration = glbAnimation->Duration;
						
					float fMaxTime = 0;
					for(const auto& channel : glbAnimation->Channels) {
						AnimationTrack track;
						track.Path = channel->Path;
						track.NodeIndex = channel->TargetNode;
						for(const auto& keyFrame : channel->KeyFrames) {
							AnimationKeyFrame kf = AnimationKeyFrame();
							BonePosition bn = BonePosition();
							bn.rotation = DirectX::XMLoadFloat4(&keyFrame.Rotation);
							bn.translation = keyFrame.Translation;
							kf.Position = bn;
							kf.Time = keyFrame.Time;
							track.Positions.push_back(kf);

							if(kf.Time > fMaxTime) {
								fMaxTime = kf.Time;
							}
						}
						animation->animation_tracks.push_back(track);
						animation->duration = fMaxTime;
					}
				}
	}

	Draw(modelAsset.pModel);
}

void CRenderer::ClearScreen() {
	m_pGraphicsEngine->ClearScreen();
}

void CRenderer::Present() {
	for(RenderCommand_t command : m_vecCommands) {
		switch(command.eType) {
			case ERenderCommandType::STATIC_MESH:
				Draw(command);
				break;
			case ERenderCommandType::SKINNED_MESH:
				Draw(command);
				break;
			case ERenderCommandType::PARTICLE_SYSTEM:
				Draw(command);
				break;
			default:
			case ERenderCommandType::NONE:
				break;
		}
	}

	m_vecCommands.clear();

	m_pGraphicsEngine->Present();
}

void CRenderer::SetFullscreen(bool bFullscreen) {
	m_pGraphicsEngine->SetFullScreen(bFullscreen);
}
	
void CRenderer::RenderPartialCover(float fX, float fY, float fWidth, float fHeight, float fCoverage) {
	float afCoverColor[4] = {0.0f, 0.0f, 1, 0.5f};
	float fHalfWidth = fWidth / 2;
	float fHalfHeight = fHeight / 2;
	if (fCoverage > 0.875) {
		float dx = 0.125 - (fCoverage - 0.875);
		float fx = 24 * (dx / 0.125);

		Vector2 points[7]{
			{ fX + fHalfWidth, fY + fHalfHeight },
			{ fX + fHalfWidth + fx, fY },
			{ fX + fWidth, fY },
			{ fX + fWidth, fY + fHeight },
			{ fX, fY + fHeight },
			{ fX, fY },
			{ fX + fHalfWidth, fY },
		};
		FillShape(points, 7, afCoverColor);
	}
	else if (fCoverage > 0.625) {
		float dx = 0.25 - (fCoverage - 0.625);
		float fx = 49 * (dx / 0.25);

		Vector2 points[6]{
			{ fX + fHalfWidth, fY + fHalfHeight },
			{ fX + fWidth, fY + fx },
			{ fX + fWidth, fY + fHeight },
			{ fX, fY + fHeight },
			{ fX, fY },
			{ fX + fHalfWidth, fY },
		};
		FillShape(points, 6, afCoverColor);
	}
	else if (fCoverage > 0.375f) {
		float dx = 0.25 - (fCoverage - 0.375);
		float fx = 49 * (dx / 0.25);

		Vector2 points[5]{
			{ fX + fHalfWidth, fY + fHalfHeight },
			{ fX + fWidth - fx, fY + fHeight },
			{ fX, fY + fHeight },
			{ fX, fY },
			{ fX + fHalfWidth, fY },
		};
		FillShape(points, 5, afCoverColor);
	}
	else if (fCoverage > 0.125f) {
		float dx = 0.25f - (fCoverage - 0.125f);
		float fx = 49 * (dx / 0.25f);

		Vector2 points[4]{
			{ fX + fHalfWidth, fY + fHalfHeight },
			{ fX, fY + fHeight - fx },
			{ fX, fY },
			{ fX + fHalfWidth, fY },
		};
		FillShape(points, 4, afCoverColor);
	}
	else if (fCoverage > 0) {
		float dx = 0.125f - fCoverage;
		float fx = 24.0f * (dx / 0.125f);

		Vector2 points[3]{
			{ fX + fHalfWidth, fY + fHalfHeight },
			{ fX + fx, fY },
			{ fX + fHalfWidth, fY },
		};
		FillShape(points, 3, afCoverColor);
	}
}

#ifdef _DEBUG
void CRenderer::RenderNavGrid(NavigationCellGrid* pNavGrid) {
	FrameConstants_t data{};
	data.cameraMatrix = cameraMatrix;
	data.projMatrix = m_projMatrix;
	m_pGraphicsEngine->UpdateBuffer(m_hFrameConstBuffer, &data, sizeof(FrameConstants_t));
	m_pGraphicsEngine->BindVertexShaderConstantBuffer(0, m_hFrameConstBuffer);

	for (int i = 0; i < pNavGrid->CellCountX * pNavGrid->CellCountY; i++) {
		NavigationCell* pCell = pNavGrid->Cells[i];
		if (pCell->IsWalkable && pCell->IsOpen) {
			continue;
		}
		ModelConstants_t modelData{};
		DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationRollPitchYaw(
			DirectX::XMConvertToRadians(0),
			DirectX::XMConvertToRadians(0),
			DirectX::XMConvertToRadians(0));
		DirectX::XMMATRIX transMat = DirectX::XMMatrixTranslation(pCell->X, 0, pCell->Y);
		DirectX::XMStoreFloat4x4(&modelData.modelMatrix, DirectX::XMMatrixTranspose(rotMat * transMat));
		m_pGraphicsEngine->UpdateBuffer(m_hModelConstBuffer, &modelData, sizeof(ModelConstants_t));
		m_pGraphicsEngine->BindVertexShaderConstantBuffer(1, m_hModelConstBuffer);

		m_pGraphicsEngine->BindShaderProgram(m_hFlatUnlitShaderProgram);

		m_pGraphicsEngine->SetVertexBuffer(0, m_pNavGridVertexBuffer, sizeof(FlatUnlitShaderVertex_t), 0);
		m_pGraphicsEngine->SetIndexBuffer(m_pNavGridIndexBuffer);

		m_pGraphicsEngine->DrawIndexed(6);
	}
}
#endif

void CRenderer::RenderChat(std::vector<std::string> vecMsgs) {
	if (vecMsgs.size() == 0) {
		return;
	}

	float afRed[3] = { 255, 0, 0 };
	std::string strMsg = vecMsgs.back();
	RenderText(100, m_height - 100, 300, 10, afRed, strMsg);
}


bool CRenderer::InitParticleEmitter(ParticleEmitter* pEmitter) {
	ParticleShaderVertex_t vertices[6] {
		{ {-1, 1, 0}, {0, 0} },
		{ {1, 1, 0}, {1, 0} },
		{ {1, -1, 0}, {1, 1} },
		{ {-1, 1, 0}, {0, 0} },
		{ {1, -1, 0}, {1, 1} },
		{ {-1, -1, 0}, {0, 1} },
	};
	int vertex_count = 6;
	pEmitter->vertex_buffer_ = m_pGraphicsEngine->CreateVertexBuffer(vertices, vertex_count, sizeof(ParticleShaderVertex_t) * vertex_count);

	if (!pEmitter->vertex_buffer_) {
		return false;
	}

	ParticleShaderVertexInstance_t* instances = new ParticleShaderVertexInstance_t[pEmitter->particle_count]{};
	pEmitter->instance_buffer_ = m_pGraphicsEngine->CreateInstanceBuffer(instances, pEmitter->particle_count, sizeof(ParticleShaderVertexInstance_t));

	if (!pEmitter->instance_buffer_) {
		return false;
	}

	return true;
}


bool CRenderer::InitParticleSystem(ParticleSystem* pSystem) {
	for (ParticleEmitter* emitter : pSystem->emitters_) {
		if (!InitParticleEmitter(emitter)) {
			return false;
		}
	}

	return true;
}

void CRenderer::Submit(RenderCommand_t command) {
	m_vecCommands.push_back(command);
}

Mesh* CRenderer::LoadMesh(GLBModelMesh* glbMesh) {
	Mesh* mesh = new Mesh();
	mesh->VertexBuffer = m_pGraphicsEngine->CreateVertexBuffer(glbMesh->Vertices.data(), glbMesh->Vertices.size() * sizeof(SkinnnedMeshShaderVertex_t), glbMesh->Vertices.size());
	mesh->IndexBuffer = m_pGraphicsEngine->CreateIndexBuffer(glbMesh->Indices.data(), glbMesh->Indices.size());
	mesh->IndexCount = glbMesh->Indices.size();
	mesh->MaterialIndex = glbMesh->MaterialIndex;
	return mesh;
}

ModelNode* CRenderer::LoadNode(GLBNode* glbNode) {
	ModelNode* modelNode = new ModelNode();
	modelNode->Mesh = glbNode->Mesh;
	modelNode->Skin = glbNode->Skin;

	for(const auto& glbChildNode : glbNode->Children) {
		modelNode->Children.push_back(glbChildNode);
	}
	return modelNode;
}

void CRenderer::Render(CGameState* pGameState) {
	for(UnitId idUnit : pGameState->vecUnits) {
		if(RenderableComponent_t* pRenderable = pGameState->GetComponent<RenderableComponent_t>(idUnit)) {
			
			HModel hModel = m_pAssetManager->LoadModel(pRenderable->strRenderable);
			ModelAsset_t& modelAsset = m_pAssetManager->GetModel(hModel);

			if(modelAsset.pModel == nullptr) {
				modelAsset.pModel = new Model();
				for(const auto& glbNode : modelAsset.pGlbModel->Nodes) {
					ModelNode* modelNode = LoadNode(glbNode.second);
					modelAsset.pModel->Nodes.emplace(glbNode.first, modelNode);
				}

				for(const auto& glbMesh : modelAsset.pGlbModel->Meshes) {
					Mesh* mesh = LoadMesh(glbMesh.second);
					modelAsset.pModel->Meshes.emplace(glbMesh.first, mesh);
				}

				for(const auto& m : modelAsset.pGlbModel->Materials) {
					Material* material = new Material();
					material->hTexture = m_pAssetManager->LoadTextureFromData(m.second->TextureData);
					modelAsset.pModel->Materials.emplace(m.first, material);
				}
							
				for(const auto& skin : modelAsset.pGlbModel->Skins) {
					Armature* armature = new Armature();
					const auto& glbSkin = skin.second;

					for(int i = 0; i < glbSkin->Joints.size(); i++) {
						const auto& joint = glbSkin->Joints[i];
						Bone bone = Bone();
						bone.Index = joint;
						armature->bones.push_back(bone);
					}

					for(int i = 0; i < glbSkin->Joints.size(); i++) {
						for(auto childIndex : modelAsset.pGlbModel->Nodes[glbSkin->Joints[i]]->Children) {
							for(int j = 0; j < armature->bones.size(); j++) {
								if(armature->bones[j].Index == childIndex) {
									armature->bones[j].parent_index = glbSkin->Joints[i];
								}
							}
						}
					}

					armature->global_inverse_bind_poses = glbSkin->InverseBindMatrices;

					modelAsset.pModel->Skins.emplace(skin.first, armature);
				}

				for(const auto& glbAnimationEntry : modelAsset.pGlbModel->Animations) {
					GLBAnimation* glbAnimation = glbAnimationEntry.second;
					Animation* animation = new Animation();
					modelAsset.pModel->Animations.emplace(glbAnimation->Name, animation);

					animation->duration = glbAnimation->Duration;
						
					float fMaxTime = 0;
					for(const auto& channel : glbAnimation->Channels) {
						AnimationTrack track;
						track.Path = channel->Path;
						track.NodeIndex = channel->TargetNode;
						for(const auto& keyFrame : channel->KeyFrames) {
							AnimationKeyFrame kf = AnimationKeyFrame();
							BonePosition bn = BonePosition();
							bn.rotation = DirectX::XMLoadFloat4(&keyFrame.Rotation);
							bn.translation = keyFrame.Translation;
							kf.Position = bn;
							kf.Time = keyFrame.Time;
							track.Positions.push_back(kf);

							if(kf.Time > fMaxTime) {
								fMaxTime = kf.Time;
							}
						}
						animation->animation_tracks.push_back(track);
						animation->duration = fMaxTime;
					}
				}
			}
			
			for(const auto& modelNode : modelAsset.pModel->Nodes) {
				Mesh* mesh = modelAsset.pModel->Meshes.at(modelNode.second->Mesh);
				if(mesh == nullptr) {
					continue;
				}
				

				RenderCommand_t cmd {
					.eType = ERenderCommandType::SKINNED_MESH,
					.worldMatrix = {},
					.hModel = hModel,
					.vecBones = {},
				};
		
				if(mesh->MaterialIndex != -1 && modelAsset.pModel->Materials.size() > mesh->MaterialIndex) {
					cmd.hTexture = modelAsset.pModel->Materials.at(mesh->MaterialIndex)->hTexture;
				}

				cmd.hVertexBuffer = mesh->VertexBuffer;
				cmd.hIndexBuffer = mesh->IndexBuffer;
				cmd.uIndexCount = mesh->IndexCount;

				if(TransformComponent_t* pTransform = pGameState->GetComponent<TransformComponent_t>(idUnit)) {
					cmd.vec3Position = pTransform->vec3Position;
					cmd.vec3Rotation = pTransform->vec3Rotation;
				}

				if(AnimationComponent_t* pAnim = pGameState->GetComponent<AnimationComponent_t>(idUnit)) {
					for(int i = 0; i < 256; i++) {
						cmd.vecBones[i] = pAnim->vecBones[i];
					}
				}
				Submit(cmd);
			}

		}
	}
}
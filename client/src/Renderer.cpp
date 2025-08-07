#include "Renderer.h"

#include "common/PloinkysJSONLibrary.h"
#include "client-asset-manager.h"

CRenderer::~CRenderer() {
	for (auto model_it : models_) {
		delete model_it.second;
	}

	if (particleShader_ != nullptr) {
		delete particleShader_;
	}

	if (glbShader_ != nullptr) {
		delete glbShader_;
	}

	if (m_pFlatUnlitShader != nullptr) {
		delete m_pFlatUnlitShader;
		m_pFlatUnlitShader = nullptr;
	}

#ifdef _DEBUG
	m_pNavGridIndexBuffer->Release();
	m_pNavGridVertexBuffer->Release();
#endif
}

void CRenderer::Initialize(HWND hWindowHandle, bool bFullScreen, CClientAssetManager* assetManager, int width_, int height_) {
	m_d3d.Initialize(hWindowHandle, bFullScreen);
	
    m_width = width_;
    m_height = height_;

    float hp = static_cast<float>(M_PI / 180.0);

    DirectX::XMStoreFloat4x4(&m_projMatrix, DirectX::XMMatrixTranspose(DirectX::XMMatrixPerspectiveFovRH(DirectX::XMConvertToRadians(m_camera.fov), (float)m_width / (float)m_height, m_camera.nearClip, m_camera.farClip)));

    // Set initial constant matrix values
    DirectX::XMStoreFloat4x4(&cameraMatrix, DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, DirectX::XMMatrixTranslation(
        m_camera.position.x, m_camera.position.y, m_camera.position.z))));

		m_pAssetManager = assetManager;
}

void CRenderer::LoadResources(AssetManager* pAssetManager) {
    // ------------ NEW ------------
    particleShader_ = new ParticleShader();
    particleShader_->Initialize(&m_d3d, pAssetManager);

	glbShader_ = new GLBShader();
	glbShader_->Initialize(&m_d3d, pAssetManager);

	m_pFlatUnlitShader = new CFlatUnlitShader();
	m_pFlatUnlitShader->Initialize(&m_d3d, pAssetManager);

    // ------------ TEXTURES ------------
	// TODO do we need this somewhere lelse
	// bitmaps_.emplace("MissingTexture", CreateBitmapFromData(pAssetManager->LoadFile("Generic\\missing_texture.bmp")));

	// ------------ IMAGES ------------
	m_pAssetManager->LoadTexture("UI/MoveTo\\move_to.png");
	m_pAssetManager->LoadTexture("Persons/ChessPerson\\blast_area.png");
	m_pAssetManager->LoadTexture("Persons/ChessPerson\\particle.png");
	m_pAssetManager->LoadTexture("characters/stormcaller/abilities\\thunderstrike.png");

	// ------------ GLB ------------
	LoadGLBModel("map1", "Maps/map1\\map1.glb", pAssetManager);
	LoadGLBModel("football_person", "Persons/ChessPerson\\chess_person.glb", pAssetManager);
	LoadGLBModel("tower", "Buildings/Tower\\tower.glb", pAssetManager);
	LoadGLBModel("missile", "Persons/ChessPerson\\missile.glb", pAssetManager);
	LoadGLBModel("minion", "Persons/Minion\\minion.glb", pAssetManager);

	LoadCharacterManifest("stormcaller", pAssetManager);

#ifdef _DEBUG
	FlatUnlitShaderVertex_t vertices[4] = {
		{{0, 10, 0}, {1, 0, 0, 1}},
		{{50, 10, 0}, {1, 0, 0, 1}},
		{{50, 10, -50}, {1, 0, 0, 1}},
		{{0, 10, -50}, {1, 0, 0, 1}},
	};

	unsigned int indices[6] = { 0, 1, 2, 0, 2, 3 };

	m_pNavGridVertexBuffer = m_d3d.CreateVertexBuffer(vertices, 4, sizeof(FlatUnlitShaderVertex_t) * 4);
	m_pNavGridIndexBuffer = m_d3d.CreateIndexBuffer(indices, 6);
#endif
}

void CRenderer::LoadCharacterManifest(std::string strCharacterId, AssetManager* pAssetManager) {
	PJL::JSONValue manifestValue = PJL::JSONParser().Parse(
		std::string((char*) pAssetManager->LoadFile("characters/" + strCharacterId + "/character_manifest.json").data())
	);

	if(!manifestValue.IsObject()) {
		Logger::Err("Failed to load manifest for character " + strCharacterId);
		return;
	}

	PJL::JSONObject manifest = manifestValue.AsObject();

	if(manifest.Contains("model") && manifest.Get("model").IsString()) {
		LoadGLBModel(strCharacterId, manifest.Get("model").AsString(), pAssetManager);
	}

	if(manifest.Contains("icons") && manifest.Get("icons").IsArray()) {
		PJL::JSONArray arrIcons = manifest.Get("icons").AsArray();
		for(int i = 0; i < arrIcons.Size(); i++) {
			PJL::JSONValue val = arrIcons.Get(i);

			if(val.IsString()) {
				// TODO this moves to asset manager
				// bitmaps_.emplace(val.AsString(), CreateBitmapFromData(pAssetManager->LoadFile(val.AsString())));
			}
		}
	}
}

Mesh* CRenderer::LoadMesh(GLBModelMesh* glbMesh) {
	Mesh* mesh = new Mesh();
	mesh->VertexBuffer = m_d3d.CreateVertexBuffer(glbMesh->Vertices.data(), glbMesh->Vertices.size(), glbMesh->Vertices.size() * sizeof(glb_shader_vertex_t));
	mesh->IndexBuffer = m_d3d.CreateIndexBuffer(glbMesh->Indices.data(), glbMesh->Indices.size());
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

void CRenderer::LoadGLBModel(std::string name, std::string file, AssetManager* assetManager) {
	Model* model = new Model();
	GLBModel* glbModel = GLBFileLoader::LoadModelFromGLBFile(file, assetManager);

	for(const auto& glbNode : glbModel->Nodes) {
		ModelNode* modelNode = LoadNode(glbNode.second);
		model->Nodes.emplace(glbNode.first, modelNode);
	}

	for(const auto& glbMesh : glbModel->Meshes) {
		Mesh* mesh = LoadMesh(glbMesh.second);
		model->Meshes.emplace(glbMesh.first, mesh);
	}

	for(const auto& m : glbModel->Materials) {
		Material* material = new Material();
		material->hTexture = m_pAssetManager->LoadTextureFromData(m.second->TextureData);
		model->Materials.emplace(m.first, material);
	}

	for(const auto& skin : glbModel->Skins) {
		Armature* armature = new Armature();
		const auto& glbSkin = skin.second;

		for(int i = 0; i < glbSkin->Joints.size(); i++) {
			const auto& joint = glbSkin->Joints[i];
			Bone bone = Bone();
			bone.Index = joint;
			armature->bones.push_back(bone);
		}

		for(int i = 0; i < glbSkin->Joints.size(); i++) {
			for(auto childIndex : glbModel->Nodes[glbSkin->Joints[i]]->Children) {
				for(int j = 0; j < armature->bones.size(); j++) {
					if(armature->bones[j].Index == childIndex) {
						armature->bones[j].parent_index = glbSkin->Joints[i];
					}
				}
			}
		}

		armature->global_inverse_bind_poses = glbSkin->InverseBindMatrices;

		model->Skins.emplace(skin.first, armature);
	}

    for(const auto& glbAnimationEntry : glbModel->Animations) {
        GLBAnimation* glbAnimation = glbAnimationEntry.second;
        Animation* animation = new Animation();
        model->Animations.emplace(glbAnimation->Name, animation);

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

	models_.emplace(name, model);
}

void CRenderer::SetDimensions(int width_, int height_) {
	m_d3d.SetWindowDimensions(width_, height_);

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

void CRenderer::UpdateBuffer(ID3D11Buffer* buffer, const void* src, size_t size) {
    D3D11_MAPPED_SUBRESOURCE mappedResource = { 0 };
    m_d3d.context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    memcpy(mappedResource.pData, src, size);
    m_d3d.context->Unmap(buffer, 0);
}

void CRenderer::RenderText(int x, int y, int w, int h, std::string text) {
    float color[3] = { 1.0, 1.0, 1.0 };
    RenderText(x, y, w, h, color, text);
}

void CRenderer::RenderText(int x, int y, int w, int h, float color[3], std::string text) {
    //Set the Font Color
    D2D1_COLOR_F FontColor = D2D1::ColorF(color[0], color[1], color[2], 1.0f);

    ID2D1SolidColorBrush* brush;
    HRESULT hr = m_d3d.renderTarget2D->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::Blue, 0.0f),
        &brush
    );

    if (FAILED(hr) || brush == nullptr) {
        return;
    }

    m_d3d.format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

    IDWriteTextLayout* textLayout;

    std::wstring wstr;
    int convertResult = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), (int)strlen(text.c_str()), NULL, 0);
    wstr.resize(convertResult);
    convertResult = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), (int)strlen(text.c_str()), &wstr[0], (int)wstr.size());

    hr = m_d3d.dWriteFactory->CreateTextLayout(
        wstr.c_str(),
        wstr.length(),
        m_d3d.format,
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
    m_d3d.renderTarget2D->DrawTextLayout(
        point,
        textLayout,
        brush
    );

    brush->Release();
    textLayout->Release();
}

void CRenderer::DrawRect(int x, int y, int w, int h, float color[3]) {
    D2D1_RECT_F rect{};
    rect.left = static_cast<float>(x);
    rect.top = static_cast<float>(y);
    rect.right = static_cast<float>(x + w);
    rect.bottom = static_cast<float>(y + h);

    //Set the Font Color
    D2D1_COLOR_F c = D2D1::ColorF(color[0], color[1], color[2]);


    ID2D1SolidColorBrush* brush;
    HRESULT hr = m_d3d.renderTarget2D->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::Black, 1.0f),
        &brush
    );


    if (FAILED(hr) || brush == 0) {
        Logger::Err("Failed to create brush for rect");
        return;
    }

    brush->SetColor(c);

    m_d3d.renderTarget2D->DrawRectangle(&rect, brush);

    brush->Release();
}

void CRenderer::DrawShape(Vector2* points, int pointCount, float color[3]) {
    //Set the Font Color
    D2D1_COLOR_F c = D2D1::ColorF(color[0], color[1], color[2]);

    ID2D1SolidColorBrush* brush;
    HRESULT hr = m_d3d.renderTarget2D->CreateSolidColorBrush(
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

    m_d3d.d2d_factory_->CreatePathGeometry(&geometry);
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

    m_d3d.renderTarget2D->DrawGeometry(geometry, brush);

    geometrySink->Release();
    geometry->Release();
    brush->Release();
};

void CRenderer::FillShape(Vector2* points, int pointCount, float color[3]) {
    if (pointCount < 2) {
        Logger::Err("Failed to draw shape: cannot draw shape from 1 point only");
        return;
    }

    //Set the Font Color
    D2D1_COLOR_F c = D2D1::ColorF(color[0], color[1], color[2]);

    ID2D1SolidColorBrush* brush;
    HRESULT hr = m_d3d.renderTarget2D->CreateSolidColorBrush(
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

    m_d3d.d2d_factory_->CreatePathGeometry(&geometry);
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

    m_d3d.renderTarget2D->FillGeometry(geometry, brush);

    geometrySink->Release();
    geometry->Release();
    brush->Release();
};

void CRenderer::FillRect(int x, int y, int w, int h, float color[3]) {
    D2D1_RECT_F rect{};
    rect.left = static_cast<float>(x);
    rect.top = static_cast<float>(y);
    rect.right = static_cast<float>(x + w);
    rect.bottom = static_cast<float>(y + h);

    //Set the Font Color
    D2D1_COLOR_F c = D2D1::ColorF(color[0], color[1], color[2], 1.0f);

    ID2D1SolidColorBrush* brush;
    HRESULT hr = m_d3d.renderTarget2D->CreateSolidColorBrush(c, &brush);


    if (FAILED(hr) || brush == 0) {
        Logger::Err("Failed to create brush for rect");
        return;
    }

    m_d3d.renderTarget2D->FillRectangle(&rect, brush);

    brush->Release();
}

void CRenderer::DrawImage(float x, float y, float w, float h, HBitmap hBitmap) {
	if(hBitmap == INVALID_ASSET_HANDLE) {
		throw std::exception("attempting to draw invalid image");
	}

	BitmapAsset_t& bitmap = m_pAssetManager->GetBitmapImage(hBitmap);

	if(bitmap.pBitmap == nullptr) {
        // Create a Direct2D bitmap from the WIC bitmap.
        m_d3d.renderTarget2D->CreateBitmapFromWicBitmap(
            bitmap.pConvertedData,
            NULL,
            &bitmap.pBitmap
        );
	}
    m_d3d.renderTarget2D->DrawBitmap(bitmap.pBitmap, D2D1::RectF(x, y, x + w, y + h));
}

template<>
void CRenderer::UpdateShaderConst(particle_shader_frame_const_t const_data) {
    UpdateBuffer(particleShader_->m_frameConstBuffer, &const_data, sizeof(particleShader_->m_frameConstData));
    m_d3d.context->VSSetConstantBuffers(0, 1, &particleShader_->m_frameConstBuffer);
}

template<>
void CRenderer::UpdateShaderConst(particle_shader_model_const_t const_data) {
    UpdateBuffer(particleShader_->m_modelConstBuffer, &const_data, sizeof(particleShader_->m_modelConstData));
    m_d3d.context->VSSetConstantBuffers(1, 1, &particleShader_->m_modelConstBuffer);
}

void CRenderer::EnableAlphaBlending() {
    m_d3d.EnableAlphaBlending();
    m_d3d.EnableDepthStencilState();
}

void CRenderer::DisableAlphaBlending() {
    m_d3d.DisableAlphaBlending();
    m_d3d.DisableDepthStencilState();
}

void DoThingsWithBones(Armature* skin, int i, std::map<int, BonePosition>& bonePositions, std::vector<DirectX::XMMATRIX>& boneTransforms) {
	const Bone& bone = skin->bones[i];
	BonePosition position = bonePositions[bone.Index];

	DirectX::XMMATRIX localTransform = DirectX::XMMatrixAffineTransformation(
		DirectX::XMVectorSplatOne(),
		DirectX::XMVectorZero(),
		position.rotation,
		DirectX::XMLoadFloat3(&position.translation)
	);

	if (bone.parent_index == -1) {
		boneTransforms[i] = localTransform;
	} else {
		for(int pi = 0; pi < skin->bones.size(); pi++) {
			const Bone& b = skin->bones[pi];
			if(b.Index == bone.parent_index) {
				boneTransforms[i] = DirectX::XMMatrixMultiply(localTransform, boneTransforms[pi]);
				break;
			}
		}
	}

	for(int otherBone = 0; otherBone < skin->bones.size(); otherBone++) {
		if(skin->bones[otherBone].parent_index == bone.Index) {
			DoThingsWithBones(skin, otherBone, bonePositions, boneTransforms);
		}
	}
}

void CRenderer::Draw(GameObject* gameObject) {
	if(ParticleSystem* ps = dynamic_cast<ParticleSystem*>(gameObject)) {
		for(auto e : ps->emitters_) {
			RenderParticle(e);
		}
		return;
	}

	// TODO collect / sort game objects for rendering?
	if(gameObject->renderable == "") {
		return; // nothing to draw
	}

	auto modelIt = models_.find(gameObject->renderable);

	if(modelIt == models_.end()) {
		Logger::FormatErr("Unable to render object, renderable <%s> not loaded", gameObject->renderable.c_str());
		return;
	}

	Model* model = modelIt->second;
		
    m_d3d.context->VSSetShader(glbShader_->m_vertexShader, 0, 0);
    m_d3d.context->PSSetShader(glbShader_->m_pixelShader, 0, 0);
    m_d3d.context->IASetInputLayout(glbShader_->m_inputLayout);
	m_d3d.context->PSSetSamplers(0, 1, &glbShader_->samplerState_);

	glb_shader_frame_const_t data{};
	data.cameraMatrix = cameraMatrix;
	data.projMatrix = m_projMatrix;
	UpdateBuffer(glbShader_->m_frameConstBuffer, &data, sizeof(glbShader_->m_frameConstData));
	m_d3d.context->VSSetConstantBuffers(0, 1, &glbShader_->m_frameConstBuffer);

	glb_shader_model_const_t model_data{};
	DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationRollPitchYaw(
		DirectX::XMConvertToRadians(gameObject->rotation.x),
		DirectX::XMConvertToRadians(gameObject->rotation.y),
		DirectX::XMConvertToRadians(gameObject->rotation.z));
	DirectX::XMMATRIX transMat = DirectX::XMMatrixTranslation(gameObject->position.x, gameObject->position.y, gameObject->position.z);
	DirectX::XMStoreFloat4x4(&model_data.modelMatrix, DirectX::XMMatrixTranspose(rotMat * transMat));
	UpdateBuffer(glbShader_->m_modelConstBuffer, &model_data, sizeof(glbShader_->m_modelConstData));
	m_d3d.context->VSSetConstantBuffers(1, 1, &glbShader_->m_modelConstBuffer);
		

	for(const auto& modelNode : model->Nodes) {
		Mesh* mesh = model->Meshes.at(modelNode.second->Mesh);
		if(mesh == nullptr) {
			continue;
		}
			
		if(gameObject->GetCurrentAnimation().GetAnimationName().length() > 0) {
			auto animIt = model->Animations.find(gameObject->GetCurrentAnimation().GetAnimationName());

			if(animIt != model->Animations.end() && model->Skins.find(modelNode.second->Skin) != model->Skins.end()) {
				Animation* animation = animIt->second;
				Armature* skin = model->Skins.at(modelNode.second->Skin);
				std::map<int, BonePosition> bonePositions = animation->GetBonePositions(gameObject->GetCurrentAnimation().GetAnimationTime(), gameObject->GetCurrentAnimation().DoLoop());
				std::vector<DirectX::XMMATRIX> boneTransforms(skin->bones.size());

				for (size_t i = 0; i < skin->bones.size(); ++i) {
					if(skin->bones[i].parent_index == -1) {
 						DoThingsWithBones(skin, i, bonePositions, boneTransforms);
					}
				}

				// Apply inverse bind pose to get the final bone transformation
				for (size_t i = 0; i < skin->bones.size(); ++i) {
  					boneTransforms[i] = DirectX::XMMatrixMultiply(skin->global_inverse_bind_poses[i], boneTransforms[i]);
				}

				for(int i = 0; i < boneTransforms.size(); i++) {
					DirectX::XMStoreFloat4x4(&glbShader_->m_meshConstData.boneTransforms[i], DirectX::XMMatrixTranspose(boneTransforms[i]));
				}
				UpdateBuffer(glbShader_->m_meshConstBuffer, &glbShader_->m_meshConstData, sizeof(glbShader_->m_meshConstData));
				m_d3d.context->VSSetConstantBuffers(2, 1, &glbShader_->m_meshConstBuffer);
			} else {
				for(int i = 0; i < 256; i++) {
					DirectX::XMStoreFloat4x4(&glbShader_->m_meshConstData.boneTransforms[i], DirectX::XMMatrixTranspose(DirectX::XMMatrixIdentity()));
				}
				UpdateBuffer(glbShader_->m_meshConstBuffer, &glbShader_->m_meshConstData, sizeof(glbShader_->m_meshConstData));
				m_d3d.context->VSSetConstantBuffers(2, 1, &glbShader_->m_meshConstBuffer);
				// TODO Logger::FormatErr("GameObject attempting to play invalid animation <%s>", gameObject->GetCurrentAnimation().GetAnimationName());
			}
		}

		if(mesh->MaterialIndex != -1 && model->Materials.size() > mesh->MaterialIndex) {
			TextureAsset_t& textureAsset = m_pAssetManager->GetTexture(model->Materials.at(mesh->MaterialIndex)->hTexture);
			if(textureAsset.pTexture == nullptr) {
				ID3D11Texture2D* texture = 0;

				D3D11_SUBRESOURCE_DATA initData = {};
				initData.pSysMem = textureAsset.data.data();
				initData.SysMemPitch = textureAsset.uWidth * 4;

				D3D11_TEXTURE2D_DESC desc = {};
				desc.Width = textureAsset.uWidth;
				desc.Height = textureAsset.uHeight;
				desc.MipLevels = 1;
				desc.ArraySize = 1;
				desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				desc.SampleDesc.Count = 1;
				desc.Usage = D3D11_USAGE_DEFAULT;
				desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

				HRESULT hr = m_d3d.device->CreateTexture2D(&desc, &initData, &texture);

				if (FAILED(hr) || texture == nullptr) {
					return;
				}

 				m_d3d.device->CreateShaderResourceView(texture, nullptr, &textureAsset.pTexture);

				texture->Release();
			}
			ID3D11ShaderResourceView* pTex = m_pAssetManager->GetTexture(model->Materials.at(mesh->MaterialIndex)->hTexture).pTexture;
			m_d3d.context->PSSetShaderResources(0, 1, &pTex);
		}
		UINT uStride = sizeof(glb_shader_vertex_t);
		UINT uOffset = 0;
		m_d3d.context->IASetVertexBuffers(0, 1, &mesh->VertexBuffer, &uStride, &uOffset);
		m_d3d.context->IASetIndexBuffer(mesh->IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		m_d3d.context->DrawIndexed(mesh->IndexCount, 0, 0);
	}
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
				ID3D11Texture2D* texture = 0;

				D3D11_SUBRESOURCE_DATA initData = {};
				initData.pSysMem = textureAsset.data.data();
				initData.SysMemPitch = textureAsset.uWidth * 4;

				D3D11_TEXTURE2D_DESC desc = {};
				desc.Width = textureAsset.uWidth;
				desc.Height = textureAsset.uHeight;
				desc.MipLevels = 1;
				desc.ArraySize = 1;
				desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				desc.SampleDesc.Count = 1;
				desc.Usage = D3D11_USAGE_DEFAULT;
				desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

				HRESULT hr = m_d3d.device->CreateTexture2D(&desc, &initData, &texture);

				if (FAILED(hr) || texture == nullptr) {
					return;
				}

				m_d3d.device->CreateShaderResourceView(texture, nullptr, &textureAsset.pTexture);

				texture->Release();
			}
			ID3D11ShaderResourceView* pTex = m_pAssetManager->GetTexture(model->Materials.at(mesh->MaterialIndex)->hTexture).pTexture;
			m_d3d.context->PSSetShaderResources(0, 1, &pTex);
		}
		UINT uStride = sizeof(glb_shader_vertex_t);
		UINT uOffset = 0;
		m_d3d.context->IASetVertexBuffers(0, 1, &mesh->VertexBuffer, &uStride, &uOffset);
		m_d3d.context->IASetIndexBuffer(mesh->IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		m_d3d.context->DrawIndexed(mesh->IndexCount, 0, 0);
	}
}

void CRenderer::RenderParticle(ParticleEmitter* emitter) {
    m_d3d.context->VSSetShader(particleShader_->m_vertexShader, 0, 0);
    m_d3d.context->PSSetShader(particleShader_->m_pixelShader, 0, 0);
    m_d3d.context->IASetInputLayout(particleShader_->m_inputLayout);
	m_d3d.context->PSSetSamplers(0, 1, &particleShader_->m_samplerState);

	float rotY = atan2(emitter->position.x - m_camera.position.x, emitter->position.z - m_camera.position.z);
	float rotX = -atan2(emitter->position.y -m_camera.position.y, m_camera.position.z- emitter->position.z);
	float rotZ = 0; // no camera roll

	if (emitter->static_angle) {
		rotX = DirectX::XMConvertToRadians(emitter->particle_angle.x);
		rotY = DirectX::XMConvertToRadians(emitter->particle_angle.y);
		rotZ = DirectX::XMConvertToRadians(emitter->particle_angle.z);
	}

	particle_shader_frame_const_t data{};
	data.cameraMatrix = cameraMatrix;
	data.projMatrix = m_projMatrix;
	DirectX::XMStoreFloat4x4(&data.billboard_matrix, DirectX::XMMatrixTranspose(DirectX::XMMatrixRotationRollPitchYaw(rotX, rotY, rotZ)));

	UpdateShaderConst<particle_shader_frame_const_t>(data);

	particle_shader_model_const_t model_data{};
	DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(emitter->rotation.x),
		DirectX::XMConvertToRadians(emitter->rotation.y),
		DirectX::XMConvertToRadians(emitter->rotation.z));
	DirectX::XMMATRIX transMat = DirectX::XMMatrixTranslation(emitter->position.x, emitter->position.y, emitter->position.z);
	DirectX::XMMATRIX scaleMat = DirectX::XMMatrixScaling(emitter->particle_scale.x, emitter->particle_scale.y, emitter->particle_scale.z);
	DirectX::XMStoreFloat4x4(&model_data.modelMatrix, DirectX::XMMatrixTranspose(scaleMat * rotMat * transMat));

	UpdateShaderConst<particle_shader_model_const_t>(model_data);

	std::vector<particle_instance_data_t> instances;

	std::sort(emitter->particles.begin(), emitter->particles.end(), [this, emitter](Particle& a, Particle& b) {
		Vector3 aVec = a.position + emitter->position;
		aVec.y = 0;
		Vector3 bVec = b.position + emitter->position;
		bVec.y = 0;
		return (m_camera.position - a.position).Length() > (m_camera.position - b.position).Length();
	});

	for (const Particle& particle : emitter->particles) {
		particle_instance_data_t p;
		p.instance_position[0] = particle.position.x;
		p.instance_position[1] = particle.position.y;
		p.instance_position[2] = particle.position.z;
		instances.push_back(p);
	}

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	// Lock the vertex buffer.
	HRESULT result = m_d3d.context->Map(emitter->instance_buffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		throw std::exception();
	}

	// Get a pointer to the data in the vertex buffer.
	particle_instance_data_t* verticesPtr = (particle_instance_data_t*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)instances.data(), (sizeof(particle_instance_data_t) * instances.size()));

	// Unlock the vertex buffer.
	m_d3d.context->Unmap(emitter->instance_buffer_, 0);

	unsigned int strides[2]{ sizeof(particle_shader_vertex_t), sizeof(particle_instance_data_t) };
	unsigned int offsets[2]{ 0, 0 };
	ID3D11Buffer* buffers[2]{ emitter->vertex_buffer_, emitter->instance_buffer_ };

	// Render this specific model
	m_d3d.context->IASetVertexBuffers(0, 2, buffers, strides, offsets);

	HTexture hTexture = m_pAssetManager->LoadTexture(emitter->texture_name_);
	TextureAsset_t& textureAsset = m_pAssetManager->GetTexture(hTexture);

	if(textureAsset.pTexture == nullptr) {
		ID3D11Texture2D* texture = 0;

		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = textureAsset.data.data();
		initData.SysMemPitch = textureAsset.uWidth * 4;

		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = textureAsset.uWidth;
		desc.Height = textureAsset.uHeight;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		HRESULT hr = m_d3d.device->CreateTexture2D(&desc, &initData, &texture);

		if (FAILED(hr) || texture == nullptr) {
			return;
		}

 		m_d3d.device->CreateShaderResourceView(texture, nullptr, &textureAsset.pTexture);

		texture->Release();
	}

	m_d3d.context->PSSetShaderResources(0, 1, &textureAsset.pTexture);
	
	EnableAlphaBlending();
	m_d3d.context->DrawInstanced(6, instances.size(), 0, 0);
	DisableAlphaBlending();
}

void CRenderer::DrawMap() {
    m_d3d.context->VSSetShader(glbShader_->m_vertexShader, 0, 0);
    m_d3d.context->PSSetShader(glbShader_->m_pixelShader, 0, 0);
    m_d3d.context->IASetInputLayout(glbShader_->m_inputLayout);
	m_d3d.context->PSSetSamplers(0, 1, &glbShader_->samplerState_);

	glb_shader_frame_const_t data{};
	data.cameraMatrix = cameraMatrix;
	data.projMatrix = m_projMatrix;
	UpdateBuffer(glbShader_->m_frameConstBuffer, &data, sizeof(glbShader_->m_frameConstData));
	m_d3d.context->VSSetConstantBuffers(0, 1, &glbShader_->m_frameConstBuffer);

	glb_shader_model_const_t model_data{};
	DirectX::XMMATRIX rotMat = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX transMat = DirectX::XMMatrixTranslation(0, 0, 0);
	DirectX::XMStoreFloat4x4(&model_data.modelMatrix, DirectX::XMMatrixTranspose(rotMat * transMat));
	UpdateBuffer(glbShader_->m_modelConstBuffer, &model_data, sizeof(glbShader_->m_modelConstData));
	m_d3d.context->VSSetConstantBuffers(1, 1, &glbShader_->m_modelConstBuffer);
		
	Model* model = models_.find("map1")->second;

	Draw(model);
}

void CRenderer::ClearScreen() {
	m_d3d.ClearScreen();
}

void CRenderer::Present() {
	m_d3d.Present();
}

void CRenderer::SetFullscreen(bool bFullscreen) {
	m_d3d.SetFullScreen(bFullscreen);
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
	FlatUnlitShaderFrameConst_t data{};
	data.cameraMatrix = cameraMatrix;
	data.projMatrix = m_projMatrix;
	UpdateBuffer(m_pFlatUnlitShader->m_pFrameConstBuffer, &data, sizeof(m_pFlatUnlitShader->m_frameConstData));
	m_d3d.context->VSSetConstantBuffers(0, 1, &m_pFlatUnlitShader->m_pFrameConstBuffer);


	for (int i = 0; i < pNavGrid->CellCountX * pNavGrid->CellCountY; i++) {
		NavigationCell* pCell = pNavGrid->Cells[i];
		if (pCell->IsWalkable && pCell->IsOpen) {
			continue;
		}
		FlatUnlitShaderObjectConst_t model_data{};
		DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationRollPitchYaw(
			DirectX::XMConvertToRadians(0),
			DirectX::XMConvertToRadians(0),
			DirectX::XMConvertToRadians(0));
		DirectX::XMMATRIX transMat = DirectX::XMMatrixTranslation(pCell->X, 0, pCell->Y);
		DirectX::XMStoreFloat4x4(&model_data.modelMatrix, DirectX::XMMatrixTranspose(rotMat * transMat));
		UpdateBuffer(m_pFlatUnlitShader->m_pModelConstBuffer, &model_data, sizeof(m_pFlatUnlitShader->m_modelConstData));
		m_d3d.context->VSSetConstantBuffers(1, 1, &m_pFlatUnlitShader->m_pModelConstBuffer);

		m_d3d.context->VSSetShader(m_pFlatUnlitShader->m_pVertexShader, nullptr, 0);
		m_d3d.context->PSSetShader(m_pFlatUnlitShader->m_pPixelShader, nullptr, 0);


		unsigned int uStride = sizeof(FlatUnlitShaderVertex_t);
		unsigned int uOffset = 0;

		m_d3d.context->IASetVertexBuffers(0, 1, &m_pNavGridVertexBuffer, &uStride, &uOffset);
		m_d3d.context->IASetIndexBuffer(m_pNavGridIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		m_d3d.context->IASetInputLayout(m_pFlatUnlitShader->m_pInputLayout);
		m_d3d.context->DrawIndexed(6, 0, 0);
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

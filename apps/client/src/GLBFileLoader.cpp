#include <GLBFileLoader.h>

#define CGLTF_IMPLEMENTATION 
#include "vendor/cgltf.h"

GLBModel* GLBFileLoader::LoadUsingLib(std::string modelName, AssetManager* assetManager) {
	std::vector<uint8_t> glbFileData = assetManager->LoadFile(modelName);

	cgltf_options options{};
	cgltf_data* data = NULL;
	cgltf_result result = cgltf_parse(&options, glbFileData.data(), glbFileData.size(), &data);

	if (result != cgltf_result_success) {
		Logger::FormatErr("Failed to load model %s; cgltf library failed to parse", modelName.c_str());
		return nullptr;
	}
	
	// Load the actual binary data (buffer->data pointers)
	result = cgltf_load_buffers(&options, data, modelName.c_str());
	if (result != cgltf_result_success) {
		Logger::FormatErr("Failed to load buffers for %s", modelName.c_str());
		cgltf_free(data);
		return nullptr;
	}

	if(data->file_type != cgltf_file_type_glb) {
		Logger::FormatErr("Failed to load model %s; unexpected file type", modelName.c_str());
		cgltf_free(data);
		return nullptr;
	}

	if(data->extensions_required_count > 0) {
		Logger::FormatErr("Failed to load model %s; requires extensions", modelName.c_str());
		cgltf_free(data);
		return nullptr;
	}
	
	std::unordered_map<const cgltf_node*, int> nodeIndexMap;
	std::unordered_map<const cgltf_skin*, int> skinIndexMap;
	std::unordered_map<const cgltf_mesh*, int> meshIndexMap;
	std::unordered_map<const cgltf_material*, int> materialIndexMap;

	for (int i = 0; i < (int)data->nodes_count; ++i) {
		nodeIndexMap[&data->nodes[i]] = i;
	}

	for (int i = 0; i < (int)data->skins_count; ++i) {
		skinIndexMap[&data->skins[i]] = i;
	}

	for (int i = 0; i < (int)data->meshes_count; ++i) {
		meshIndexMap[&data->meshes[i]] = i;
	}

	for (int i = 0; i < (int)data->materials_count; ++i) {
		materialIndexMap[&data->materials[i]] = i;
	}


	GLBModel myModel = GLBModel();

	// start filling the model with data
	for(int i = 0; i < data->nodes_count; i++) {
		cgltf_node& node = data->nodes[i];

		GLBNode myNode{};
		myNode.NodeIndex = i;
		myNode.Name = node.name;
		myNode.Skin = skinIndexMap[node.skin];
		myNode.Mesh = meshIndexMap[node.mesh];
		
		if(node.has_rotation) {
			myNode.Rotation = Vector4(node.rotation[0], node.rotation[1], node.rotation[2], node.rotation[3]);
		}
		
		if(node.has_scale) {
			myNode.Scale = Vector3(node.scale[0], node.scale[1], node.scale[2]);
		}

		if(node.has_translation) {
			myNode.Translation = Vector3(node.translation[0], node.translation[1], node.translation[2]);
		}

		for(int c = 0; c < node.children_count; c++) {
			myNode.Children.emplace_back(nodeIndexMap[node.children[c]]);
		}

		myModel.Nodes.emplace(i, myNode);
	}

	for(int i = 0; i < data->meshes_count; i++) {
		cgltf_mesh& mesh = data->meshes[i];

		GLBModelMesh myMesh{};

		// TODO: we assume one primitive per mesh
		const cgltf_primitive& prim = mesh.primitives[0];

		if(prim.attributes_count != 3) {
			Logger::FormatMsg("Warning loading model %s; unexpected number of attributes", modelName.c_str());
		}

		const cgltf_accessor* pPosAccessor = nullptr;
		const cgltf_accessor* pNormAccessor = nullptr;
		const cgltf_accessor* pUVAccessor = nullptr;

		for (size_t j = 0; j < prim.attributes_count; ++j) {
			const cgltf_attribute& attr = prim.attributes[j];
			if (attr.type == cgltf_attribute_type_position)
				pPosAccessor = attr.data;
			else if (attr.type == cgltf_attribute_type_normal)
				pNormAccessor = attr.data;
			else if (attr.type == cgltf_attribute_type_texcoord)
				pUVAccessor = attr.data;
		}

    	myMesh.Vertices.resize(pPosAccessor->count);
		
		for (size_t v = 0; v < pPosAccessor->count; ++v) {
			GLBVertexType vert{};

			float pos[3] = {};
			cgltf_accessor_read_float(pPosAccessor, v, pos, 3);
			vert.Position = Vector3(pos[0], pos[1], pos[2]);

			if (pNormAccessor) {
				float nrm[3] = {};
				cgltf_accessor_read_float(pNormAccessor, v, nrm, 3);
				vert.Normals = Vector3(nrm[0], nrm[1], nrm[2]);
			}

			if (pUVAccessor) {
				float uv[2] = {};
				cgltf_accessor_read_float(pUVAccessor, v, uv, 2);
				vert.TextureCoordinates = Vector2(uv[0], uv[1]);
			}

			myMesh.Vertices[v] = vert;
		}

		if(prim.indices) {
			myMesh.Indices.resize(prim.indices->count);
			for(int c = 0; c < prim.indices->count; c++) {
				uint32_t val = 0;
				cgltf_accessor_read_uint(prim.indices, c, &val, 1);
				myMesh.Indices[c] = val;
			}
		}

		myMesh.MaterialIndex = materialIndexMap[prim.material];

		myModel.Meshes.emplace(i, myMesh);
	}

	for(int i = 0; i < data->skins_count; i++) {
		cgltf_skin skin = data->skins[i];

		GLBModelSkin mySkin{};
		mySkin.Name = skin.name;

		mySkin.Joints.resize(skin.joints_count);
		for(int c = 0; c < skin.joints_count; c++) {
			mySkin.Joints[c] = nodeIndexMap[skin.joints[c]];
		}

		const cgltf_accessor* pInvBinMatAccessor = skin.inverse_bind_matrices;
		mySkin.InverseBindMatrices.resize(pInvBinMatAccessor->count);
		for(cgltf_size c = 0; c < pInvBinMatAccessor->count; c++) {
			float raw[16];
			cgltf_accessor_read_float(pInvBinMatAccessor, c, raw, 16);

			// TODO: does 'mat' constructor take 16 floats row-major?
			mySkin.InverseBindMatrices[c].m[0][0] = raw[0];
			mySkin.InverseBindMatrices[c].m[0][1] = raw[1];
			mySkin.InverseBindMatrices[c].m[0][2] = raw[2];
			mySkin.InverseBindMatrices[c].m[0][3] = raw[3];
			mySkin.InverseBindMatrices[c].m[1][0] = raw[4];
			mySkin.InverseBindMatrices[c].m[1][1] = raw[5];
			mySkin.InverseBindMatrices[c].m[1][2] = raw[6];
			mySkin.InverseBindMatrices[c].m[1][3] = raw[7];
			mySkin.InverseBindMatrices[c].m[2][0] = raw[8];
			mySkin.InverseBindMatrices[c].m[2][1] = raw[9];
			mySkin.InverseBindMatrices[c].m[2][2] = raw[10];
			mySkin.InverseBindMatrices[c].m[2][3] = raw[11];
			mySkin.InverseBindMatrices[c].m[3][0] = raw[12];
			mySkin.InverseBindMatrices[c].m[3][1] = raw[13];
			mySkin.InverseBindMatrices[c].m[3][2] = raw[14];
			mySkin.InverseBindMatrices[c].m[3][3] = raw[15];
		}

		myModel.Skins[i] = mySkin;
	}

	for(int i = 0; i < data->materials_count; i++) {
		cgltf_material material = data->materials[i];

		GLBModelMaterial myMaterial{};

		cgltf_texture* pTexture = material.pbr_metallic_roughness.base_color_texture.texture;

		if(pTexture && pTexture->image) {
			std::vector<uint8_t> bytes;
			if(pTexture->image->uri) {
				myMaterial.TextureUri = pTexture->image->uri;
				myMaterial.TextureData = assetManager->LoadFile(myMaterial.TextureUri);

			} else if (pTexture->image->buffer_view) {
				const cgltf_buffer_view* bv = pTexture->image->buffer_view;
				const uint8_t* data = reinterpret_cast<const uint8_t*>(bv->buffer->data) + bv->offset;
				size_t size = bv->size;
				std::vector<uint8_t> bytes(data, data + size);
				myMaterial.TextureData = bytes;
			}
		}

		myModel.Materials.emplace(i, myMaterial);
	}

	for(int i = 0; i < data->animations_count; i++) {
		cgltf_animation animation = data->animations[i];

		GLBAnimation myAnimation{};
		myAnimation.Name = animation.name;

		for(int c = 0; c < animation.channels_count; c++) {
			cgltf_animation_channel channel = animation.channels[c];
			
			GLBAnimationChannel myChannel{};
			myChannel.TargetNode = nodeIndexMap[channel.target_node];

			switch(channel.target_path) {
				case cgltf_animation_path_type_translation:
					myChannel.Path = eGlbAnimationChannel_Translation;
					break;
				case cgltf_animation_path_type_rotation:
					myChannel.Path = eGlbAnimationChannel_Rotation;
					break;
				case cgltf_animation_path_type_scale:
					myChannel.Path = eGlbAnimationChannel_Scale;
					break;
				case cgltf_animation_path_type_weights:
					// TODO
				case cgltf_animation_path_type_max_enum:
				case cgltf_animation_path_type_invalid:
				default:
					continue;
			}

			cgltf_animation_sampler* pSampler = channel.sampler;

			const cgltf_accessor* pTimeAccessor = pSampler->input;
			std::vector<float> times(pTimeAccessor->count);
			for(int ac = 0; ac < pTimeAccessor->count; ac++) {
				cgltf_accessor_read_float(pTimeAccessor, ac, times.data(), 1);
			}

			// Load values
			const cgltf_accessor* pValueAccessor = pSampler->output;
			size_t valueCount = pValueAccessor->count;
			size_t components = cgltf_num_components(pValueAccessor->type);

			myChannel.KeyFrames.resize(valueCount);
			for (size_t i = 0; i < valueCount; ++i) {
				myChannel.KeyFrames[i].Time = times[i];

				switch (channel.target_path) {
					case cgltf_animation_path_type_translation: {
						float v[3];
						cgltf_accessor_read_float(pValueAccessor, i, v, 3);
						myChannel.KeyFrames[i].Translation = { v[0], v[1], v[2] };
						break;
					}
					case cgltf_animation_path_type_rotation: {
						float v[4];
						cgltf_accessor_read_float(pValueAccessor, i, v, 4);
						myChannel.KeyFrames[i].Rotation = { v[0], v[1], v[2], v[3] };
						break;
					}
					case cgltf_animation_path_type_scale: {
						float v[3];
						cgltf_accessor_read_float(pValueAccessor, i, v, 3);
						myChannel.KeyFrames[i].Scale = { v[0], v[1], v[2] };
						break;
					}
					default: break;
				}

				myAnimation.Duration = std::max(myAnimation.Duration, myChannel.KeyFrames[i].Time);
			}

			myAnimation.Channels.push_back(myChannel);
		}

		myModel.Animations.emplace(myAnimation.Name, myAnimation);
	}

	cgltf_free(data);

	GLBModel* pModel = new GLBModel(myModel);
	return pModel;
}

GLBModel* GLBFileLoader::LoadModelFromGLBFile(std::string modelName, AssetManager* assetManager) {
	std::vector<uint8_t> glbFileData = assetManager->LoadFile(modelName);
	if(glbFileData.size() < 20) {
		throw new std::runtime_error("Failed to load model <" + modelName + ">: file too small to be valid");
	}

	std::string magic = std::string(glbFileData.data(), glbFileData.data() + 4);
	if(magic.compare("glTF") != 0) {
		throw new std::runtime_error("Failed to load model <" + modelName + ">: incorrect magic");
	}

	uint32_t version, length;
	std::memcpy(&version, glbFileData.data() + 4, 4);
	std::memcpy(&length, glbFileData.data() + 8, 4);

	Logger::FormatMsg("Loading model <%s>, size: <%u>", modelName.c_str(), length);

	uint32_t jsonChunkType, jsonChunkLength;
	std::memcpy(&jsonChunkLength, glbFileData.data() + 12, 4);
	std::memcpy(&jsonChunkType, glbFileData.data() + 16, 4);

	if(jsonChunkType != 0x4E4F534A) {
		throw new std::runtime_error("Failed to load model <" + modelName + ">: not a json chunk at 0");
	}

	PJL::JSONValue jsonValue = PJL::JSONParser().Parse(std::string(glbFileData.data() + 20, glbFileData.data() + 20 + jsonChunkLength));
	if(!jsonValue.IsObject()) {
		throw new std::runtime_error("Failed to load model <" + modelName + ">: json chunk not a json object");
	}

	uint32_t binChunkType, binChunkLength;
	std::memcpy(&binChunkLength, glbFileData.data() + 20 + jsonChunkLength, 4);
	std::memcpy(&binChunkType, glbFileData.data() + 20 + jsonChunkLength + 4, 4);

	if(binChunkType != 0x004E4942) {
		throw new std::runtime_error("Failed to load model <" + modelName + ">: not a binary chunk at 1");
	}

	std::vector<uint8_t> binaryData(glbFileData.data() + 20 + jsonChunkLength + 8, glbFileData.data() + 20 + jsonChunkLength + 8 + binChunkLength);

	PJL::JSONObject glbHeader = jsonValue.AsObject();

	if(!glbHeader.Contains("meshes") || !glbHeader.Get("meshes").IsArray()) {
		throw new std::runtime_error("Failed to load model <" + modelName + ">: no meshes or not an array");
	}

	int defaultSceneIndex = glbHeader.Get("scene").AsInt();

	PJL::JSONArray scenes = glbHeader.Get("scenes").AsArray();

	PJL::JSONObject defaultScene = scenes.Get(defaultSceneIndex).AsObject();

	PJL::JSONArray defaultSceneNodeIndices = defaultScene.Get("nodes").AsArray();

	GLBModel* model = new GLBModel();

	PJL::JSONArray glbNodes = glbHeader.Get("nodes").AsArray();
	for(int i = 0; i < glbNodes.Size(); i++) {
		GLBNode node = LoadNodeFromGLBFile(i, glbHeader, binaryData);
		model->Nodes.emplace(i, node);
	}

	PJL::JSONArray glbMeshes = glbHeader.Get("meshes").AsArray();
	for(int i = 0; i < glbMeshes.Size(); i++) {
		GLBModelMesh mesh = LoadMeshFromGLBFile(i, glbHeader, binaryData);
		model->Meshes.emplace(i, mesh);
	}

	if(glbHeader.Contains("skins")) {
		PJL::JSONArray glbSkins = glbHeader.Get("skins").AsArray();
		for(int i = 0; i < glbSkins.Size(); i++) {
			GLBModelSkin skin = LoadSkinFromGLBFile(i, glbHeader, binaryData);
			model->Skins.emplace(i, skin);
		}
	}

	if(glbHeader.Contains("materials")) {
		const PJL::JSONArray& glbMaterials = glbHeader.Get("materials").AsArray();
		for(int i = 0; i < glbMaterials.Size(); i++) {
			PJL::JSONObject glbMaterial = glbMaterials.Get(i).AsObject();
			GLBModelMaterial material = GLBModelMaterial();

			if (glbMaterial.Contains("pbrMetallicRoughness")) {
				const auto& pbr = glbMaterial.Get("pbrMetallicRoughness").AsObject();

				if (pbr.Contains("baseColorFactor")) {
					// TODO
				}

				if (pbr.Contains("baseColorTexture")) {
					int textureIndex = pbr.Get("baseColorTexture").AsObject().Get("index").AsInt();
					LoadImageData(glbHeader, binaryData, textureIndex, material.TextureData);
				}
			}
			model->Materials.emplace(i, material);
		}
	}

	if(glbHeader.Contains("animations")) {
		const PJL::JSONArray& glbAnimations = glbHeader.Get("animations").AsArray();

		for(int animationIndex = 0; animationIndex < glbAnimations.Size(); animationIndex++) {
			const PJL::JSONObject glbAnimation = glbAnimations.Get(animationIndex).AsObject();
				
			GLBAnimation animation = GLBAnimation();
			animation.Name = glbAnimation.Get("name").AsString();

			const PJL::JSONArray& glbChannels = glbAnimation.Get("channels").AsArray();

			std::vector<std::vector<float>> samplerInputs;
			std::vector<std::vector<Vector4>> samplerOutputs;
			const PJL::JSONArray glbSamplers = glbAnimation.Get("samplers").AsArray();
			for (int samplerIndex = 0; samplerIndex < glbSamplers.Size(); samplerIndex++) {
				const PJL::JSONObject& glbSampler = glbSamplers.Get(samplerIndex).AsObject();
				uint32_t inputIndex = glbSampler.Get("input").AsInt();
				uint32_t outputIndex = glbSampler.Get("output").AsInt();
					
				std::vector<float> inputTimes;
				std::vector<Vector4> outputValues;
					
				LoadAttributeData(glbHeader, binaryData, inputIndex, inputTimes);
				LoadAttributeData(glbHeader, binaryData, outputIndex, outputValues);
					
				samplerInputs.push_back(inputTimes);
				samplerOutputs.push_back(outputValues);
			}

			for(int channelIndex = 0; channelIndex < glbChannels.Size(); channelIndex++) {
				const PJL::JSONObject& glbChannel = glbChannels.Get(channelIndex).AsObject();
				const int& samplerIndex = glbChannel.Get("sampler").AsInt();
				const PJL::JSONObject& glbChannelTarget = glbChannel.Get("target").AsObject();
				const int& channelTargetNodeIndex = glbChannelTarget.Get("node").AsInt();
				const std::string& path = glbChannelTarget.Get("path").AsString();

				GLBAnimationChannel channel = GLBAnimationChannel();
				channel.TargetNode = channelTargetNodeIndex;
					
				const auto& inputTimes = samplerInputs[samplerIndex];
				const auto& outputValues = samplerOutputs[samplerIndex];

				for (size_t i = 0; i < inputTimes.size(); ++i) {
					float time = inputTimes[i];
					const Vector4& value = outputValues[i];
						
					if (path == "translation") {
						channel.Path = eGlbAnimationChannel_Translation;
						channel.KeyFrames.push_back(GLBKeyFrame{time, {value.x, value.y, value.z}, {}, {1.0f, 1.0f, 1.0f}});
					} else if (path == "rotation") {
						channel.Path = eGlbAnimationChannel_Rotation;
						channel.KeyFrames.push_back(GLBKeyFrame{time, {}, {value.x, value.y, value.z, value.w}, {1.0f, 1.0f, 1.0f}});
					} else if (path == "scale") {
						channel.Path = eGlbAnimationChannel_Scale;
						channel.KeyFrames.push_back(GLBKeyFrame{time, {}, {}, {value.x, value.y, value.z}});
					}
					animation.Duration = std::max(animation.Duration, channel.KeyFrames[i].Time);
				}

 				animation.Channels.push_back(channel);
			}
			
			model->Animations.emplace(animation.Name, animation);
		}
	}

	Logger::Msg("good so far!");

	return model;
}

	
GLBNode GLBFileLoader::LoadNodeFromGLBFile(int nodeIndex, const PJL::JSONObject& glbHeader, const std::vector<uint8_t>& binaryData) {
	GLBNode node = GLBNode();
	node.NodeIndex = nodeIndex;
		
	PJL::JSONObject glbNode = glbHeader.Get("nodes").AsArray().Get(nodeIndex).AsObject();

	if(glbNode.Contains("name")) {
		node.Name = glbNode.Get("name").AsString();
	}

	if(glbNode.Contains("children")) {
		PJL::JSONArray childNodeIndices = glbNode.Get("children").AsArray();

		for(int childIndex = 0; childIndex < childNodeIndices.Size(); childIndex++) {
			node.Children.push_back(childNodeIndices.Get(childIndex).AsInt());
		}
	}

	if(glbNode.Contains("rotation")) {
		PJL::JSONArray rotationArray = glbNode.Get("rotation").AsArray();
		node.Rotation.x = rotationArray.Get(0).AsDouble();
		node.Rotation.y = rotationArray.Get(1).AsDouble();
		node.Rotation.z = rotationArray.Get(2).AsDouble();
		node.Rotation.w = rotationArray.Get(3).AsDouble();
	}

	if(glbNode.Contains("scale")) {
		PJL::JSONArray scaleArray = glbNode.Get("scale").AsArray();
		node.Scale.x = scaleArray.Get(0).AsDouble();
		node.Scale.y = scaleArray.Get(1).AsDouble();
		node.Scale.z = scaleArray.Get(2).AsDouble();
	}

	if(glbNode.Contains("translation")) {
		PJL::JSONArray translationArray = glbNode.Get("translation").AsArray();
		node.Translation.x = translationArray.Get(0).AsDouble();
		node.Translation.y = translationArray.Get(1).AsDouble();
		node.Translation.z = translationArray.Get(2).AsDouble();
	}

	if(!glbNode.Contains("mesh")) {
		return node;
	}

	node.Mesh = glbNode.Get("mesh").AsInt();

	if(!glbNode.Contains("skin")) {
		return node;
	}

	node.Skin = glbNode.Get("skin").AsInt();

	return node;
}

GLBModelMesh GLBFileLoader::LoadMeshFromGLBFile(int meshIndex, const PJL::JSONObject& glbHeader, const std::vector<uint8_t>& binaryData) {
	const PJL::JSONArray& glbMeshes = glbHeader.Get("meshes").AsArray();

	const auto& glbMesh = glbMeshes.Get(meshIndex);

	GLBModelMesh glbM = GLBModelMesh();

	if(!glbMesh.IsObject() || !glbMesh.AsObject().Contains("primitives") || !glbMesh.AsObject().Get("primitives").IsArray()) {
		throw new std::runtime_error("Failed to load model, found mesh without primitives");
	}

	const auto& glbPrimitives = glbMesh.AsObject().Get("primitives").AsArray();

	for(int j = 0; j < glbPrimitives.Size(); j++) {
		std::vector<Vector3> positions;
		std::vector<Vector3> normals;
		std::vector<Vector2> texCoords;
		std::vector<UBYTE4> joints;
		std::vector<Vector4> weights {0};

		const auto& glbPrimitive = glbPrimitives.Get(j).AsObject();

		if(!glbPrimitive.Contains("attributes") || !glbPrimitive.Get("attributes").IsObject()) {
			throw new std::runtime_error("Failed to load model, found mesh with primitives without attributes");
		}

		const auto& attributes = glbPrimitive.Get("attributes").AsObject();
		if (attributes.Contains("POSITION")) {
			int accessorIndex = attributes.Get("POSITION").AsInt();
			// Load positions using the accessorIndex...
			LoadAttributeData(glbHeader, binaryData, accessorIndex, positions);
		}
		
		if (attributes.Contains("NORMAL")) {
			int accessorIndex = attributes.Get("NORMAL").AsInt();
			// Load normals using the accessorIndex...
			LoadAttributeData(glbHeader, binaryData, accessorIndex, normals);
		}
		
		if (attributes.Contains("TEXCOORD_0")) {
			int accessorIndex = attributes.Get("TEXCOORD_0").AsInt();
			// Load texCoords using the accessorIndex...
			LoadAttributeData(glbHeader, binaryData, accessorIndex, texCoords);
		}
			
		if(attributes.Contains("JOINTS_0")) {
			int accessorIndex = attributes.Get("JOINTS_0").AsInt();
			LoadAttributeData(glbHeader, binaryData, accessorIndex, joints);
		}

		if(attributes.Contains("WEIGHTS_0")) {
			int accessorIndex = attributes.Get("WEIGHTS_0").AsInt();
			LoadAttributeData(glbHeader, binaryData, accessorIndex, weights);
		}

		if (glbPrimitive.Contains("material") && glbPrimitive.Get("material").IsInt()) {
			glbM.MaterialIndex = glbPrimitive.Get("material").AsInt();
		}

		// Interleave vertex data
		size_t vertexCount = positions.size();
		glbM.Vertices.resize(vertexCount);
		for (size_t i = 0; i < vertexCount; ++i) {
			glbM.Vertices[i].Position = positions[i];
			if (!normals.empty()) {
				glbM.Vertices[i].Normals = normals[i];
			}
			if (!texCoords.empty()) {
				glbM.Vertices[i].TextureCoordinates = texCoords[i];
			}

			if(!joints.empty()) {
				glbM.Vertices[i].Joints = joints[i];
			} else {
				glbM.Vertices[i].Joints = {0, 0, 0, 0};
			}

			if(!joints.empty()) {
				glbM.Vertices[i].Weights = weights[i];
			} else {
				glbM.Vertices[i].Weights = {0, 0, 0, 0};
			}
		}

		if (glbPrimitive.Contains("indices")) {
			int accessorIndex = glbPrimitive.Get("indices").AsInt();
			// Load indices using the accessorIndex...
			LoadIndexData(glbHeader, binaryData, accessorIndex, glbM.Indices);
		}
	}

	return glbM;
}


GLBModelSkin GLBFileLoader::LoadSkinFromGLBFile(int skinIndex, const PJL::JSONObject& glbHeader, const std::vector<uint8_t>& binaryData) {
	GLBModelSkin skin = GLBModelSkin();

	PJL::JSONObject glbSkin = glbHeader.Get("skins").AsArray().Get(skinIndex).AsObject();

	PJL::JSONArray glbJoints = glbSkin.Get("joints").AsArray();
	for(int i = 0; i < glbJoints.Size(); i++) {
		skin.Joints.push_back(glbJoints.Get(i).AsInt());
	}

	if(!glbSkin.Contains("inverseBindMatrices")) {
		return skin;
	}
		
	int glbBindMatricesIndex = glbSkin.Get("inverseBindMatrices").AsInt();

	PJL::JSONValue val = glbHeader.Get("inverseBindMatrices");
	LoadAttributeData(glbHeader, binaryData, glbBindMatricesIndex, skin.InverseBindMatrices);

	return skin;
}

template <typename T>
void GLBFileLoader::LoadAttributeData(const PJL::JSONObject& gltfJson, const std::vector<uint8_t>& binaryData, int accessorIndex, std::vector<T>& attributeData) {
	const auto& accessors = gltfJson.Get("accessors").AsArray();
	const auto& bufferViews = gltfJson.Get("bufferViews").AsArray();
	const auto& accessor = accessors.Get(accessorIndex).AsObject();
	const auto& bufferView = bufferViews.Get(accessor.Get("bufferView").AsInt()).AsObject();

	uint32_t byteOffset = bufferView.Get("byteOffset").AsInt() + (accessor.Contains("byteOffset") ? accessor.Get("byteOffset").AsInt() : 0);
	uint32_t byteStride = bufferView.Contains("byteStride") ? bufferView.Get("byteStride").AsInt() : 0;
	uint32_t componentType = accessor.Get("componentType").AsInt();
	uint32_t count = accessor.Get("count").AsInt();
	std::string type = accessor.Get("type").AsString();

	const uint8_t* data = binaryData.data() + byteOffset;

	attributeData.resize(count);

	// Assuming the componentType is FLOAT (5126) and type is VEC3 or VEC2
	if (componentType == 5126) {
		if (type == "VEC4") {
			for (uint32_t i = 0; i < count; ++i) {
				std::memcpy(&attributeData[i], data + i * (byteStride ? byteStride : sizeof(Vector4)), sizeof(Vector4));
			}
		} else if (type == "VEC3") {
			for (uint32_t i = 0; i < count; ++i) {
				std::memcpy(&attributeData[i], data + i * (byteStride ? byteStride : sizeof(Vector3)), sizeof(Vector3));
			}
		} else if (type == "VEC2") {
			for (uint32_t i = 0; i < count; ++i) {
				std::memcpy(&attributeData[i], data + i * (byteStride ? byteStride : sizeof(Vector2)), sizeof(Vector2));
			}
		} else if (type == "MAT4") {
			for (uint32_t i = 0; i < count; ++i) {
				std::memcpy(&attributeData[i], data + i * (byteStride ? byteStride : sizeof(mat)), sizeof(mat));
			}
		} else if (type == "SCALAR") {
			for (uint32_t i = 0; i < count; ++i) {
				std::memcpy(&attributeData[i], data + i * (byteStride ? byteStride : sizeof(float)), sizeof(float));
			}
		}
	} else if(componentType = 5121) {
		if (type == "VEC4") {
			for (uint32_t i = 0; i < count; ++i) {
				std::memcpy(&attributeData[i], data + i * (byteStride ? byteStride : sizeof(UBYTE4)), sizeof(UBYTE4));
			}
		} else if (type == "VEC3") {
			for (uint32_t i = 0; i < count; ++i) {
				std::memcpy(&attributeData[i], data + i * (byteStride ? byteStride : sizeof(Vector3)), sizeof(Vector3));
			}
		}
	}
}

void GLBFileLoader::LoadIndexData(const PJL::JSONObject& gltfJson, const std::vector<uint8_t>& binaryData, int accessorIndex, std::vector<uint32_t>& indices) {
	const auto& accessors = gltfJson.Get("accessors").AsArray();
	const auto& bufferViews = gltfJson.Get("bufferViews").AsArray();
	const auto& accessor = accessors.Get(accessorIndex).AsObject();
	const auto& bufferView = bufferViews.Get(accessor.Get("bufferView").AsInt()).AsObject();

	uint32_t byteOffset = bufferView.Get("byteOffset").AsInt() + (accessor.Contains("byteOffset") ? accessor.Get("byteOffset").AsInt() : 0);
	uint32_t componentType = accessor.Get("componentType").AsInt();
	uint32_t count = accessor.Get("count").AsInt();

	const uint8_t* data = binaryData.data() + byteOffset;

	indices.resize(count);
		
	if(componentType == 5123) {
		for(int i = 0; i < count; i++) {
			indices[i] = reinterpret_cast<const uint16_t*>(data)[i];
		}
	} else if (componentType == 5125) {
		for(int i = 0; i < count; i++) {
			indices[i] = reinterpret_cast<const uint32_t*>(data)[i];
		}
	}
}

void GLBFileLoader::LoadImageData(const PJL::JSONObject& gltfJson, const std::vector<uint8_t>& binaryData, int imageIndex, std::vector<uint8_t>& imageData) {
	const auto& images = gltfJson.Get("images").AsArray();
	const auto& image = images.Get(imageIndex).AsObject();
	const auto& bufferViews = gltfJson.Get("bufferViews").AsArray();
	const auto& bufferView = bufferViews.Get(image.Get("bufferView").AsInt()).AsObject();

	uint32_t byteOffset = bufferView.Get("byteOffset").AsInt() + (image.Contains("byteOffset") ? image.Get("byteOffset").AsInt() : 0);
	uint32_t byteLength = bufferView.Get("byteLength").AsInt();

	imageData.resize(byteLength);
	std::memcpy(imageData.data(), binaryData.data() + byteOffset, byteLength);
}
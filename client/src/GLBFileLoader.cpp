#include <GLBFileLoader.h>

#define CGLTF_IMPLEMENTATION 
#include "vendor/cgltf.h"

/*
GLBModel* GLBFileLoader::LoadUsingLib(std::string modelName, AssetManager* assetManager) {
	std::vector<uint8_t> glbFileData = assetManager->LoadFile(modelName);


	cgltf_options options = {0};
	cgltf_data* data = NULL;
	cgltf_result result = cgltf_parse(&options, glbFileData.data(), glbFileData.size(), &data);
	if (result == cgltf_result_success)
	{
		cgltf_free(data);
	}
}
*/

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
		GLBNode* node = LoadNodeFromGLBFile(i, glbHeader, binaryData);
		model->Nodes.emplace(i, node);
	}

	PJL::JSONArray glbMeshes = glbHeader.Get("meshes").AsArray();
	for(int i = 0; i < glbMeshes.Size(); i++) {
		GLBModelMesh* mesh = LoadMeshFromGLBFile(i, glbHeader, binaryData);
		model->Meshes.emplace(i, mesh);
	}

	if(glbHeader.Contains("skins")) {
		PJL::JSONArray glbSkins = glbHeader.Get("skins").AsArray();
		for(int i = 0; i < glbSkins.Size(); i++) {
			GLBModelSkin* skin = LoadSkinFromGLBFile(i, glbHeader, binaryData);
			model->Skins.emplace(i, skin);
		}
	}

	if(glbHeader.Contains("materials")) {
		const PJL::JSONArray& glbMaterials = glbHeader.Get("materials").AsArray();
		for(int i = 0; i < glbMaterials.Size(); i++) {
			PJL::JSONObject glbMaterial = glbMaterials.Get(i).AsObject();
			GLBModelMaterial* material = new GLBModelMaterial();

			if (glbMaterial.Contains("pbrMetallicRoughness")) {
				const auto& pbr = glbMaterial.Get("pbrMetallicRoughness").AsObject();

				if (pbr.Contains("baseColorFactor")) {
					// TODO
				}

				if (pbr.Contains("baseColorTexture")) {
					int textureIndex = pbr.Get("baseColorTexture").AsObject().Get("index").AsInt();
					LoadImageData(glbHeader, binaryData, textureIndex, material->TextureData);
				}
			}
			model->Materials.emplace(i, material);
		}
	}

	if(glbHeader.Contains("animations")) {
		const PJL::JSONArray& glbAnimations = glbHeader.Get("animations").AsArray();

		for(int animationIndex = 0; animationIndex < glbAnimations.Size(); animationIndex++) {
			const PJL::JSONObject glbAnimation = glbAnimations.Get(animationIndex).AsObject();
				
			GLBAnimation* animation = new GLBAnimation();
			animation->Name = glbAnimation.Get("name").AsString();
			model->Animations.emplace(animation->Name, animation);

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

				GLBAnimationChannel* channel = new GLBAnimationChannel();
				channel->TargetNode = channelTargetNodeIndex;
					
				const auto& inputTimes = samplerInputs[samplerIndex];
				const auto& outputValues = samplerOutputs[samplerIndex];

				for (size_t i = 0; i < inputTimes.size(); ++i) {
					float time = inputTimes[i];
					const Vector4& value = outputValues[i];
						
					if (path == "translation") {
						channel->Path = eGlbAnimationChannel_Translation;
						channel->KeyFrames.push_back(GLBKeyFrame{time, {value.x, value.y, value.z}, {}, {1.0f, 1.0f, 1.0f}});
					} else if (path == "rotation") {
						channel->Path = eGlbAnimationChannel_Rotation;
						channel->KeyFrames.push_back(GLBKeyFrame{time, {}, {value.x, value.y, value.z, value.w}, {1.0f, 1.0f, 1.0f}});
					} else if (path == "scale") {
						channel->Path = eGlbAnimationChannel_Scale;
						channel->KeyFrames.push_back(GLBKeyFrame{time, {}, {}, {value.x, value.y, value.z}});
					}
				}

 				animation->Channels.push_back(channel);
			}
		}
	}

	Logger::Msg("good so far!");

	return model;
}

	
GLBNode* GLBFileLoader::LoadNodeFromGLBFile(int nodeIndex, const PJL::JSONObject& glbHeader, const std::vector<uint8_t>& binaryData) {
	GLBNode* node = new GLBNode();
	node->NodeIndex = nodeIndex;
		
	PJL::JSONObject glbNode = glbHeader.Get("nodes").AsArray().Get(nodeIndex).AsObject();

	if(glbNode.Contains("name")) {
		node->Name = glbNode.Get("name").AsString();
	}

	if(glbNode.Contains("children")) {
		PJL::JSONArray childNodeIndices = glbNode.Get("children").AsArray();

		for(int childIndex = 0; childIndex < childNodeIndices.Size(); childIndex++) {
			node->Children.push_back(childNodeIndices.Get(childIndex).AsInt());
		}
	}

	if(glbNode.Contains("rotation")) {
		PJL::JSONArray rotationArray = glbNode.Get("rotation").AsArray();
		node->Rotation.x = rotationArray.Get(0).AsDouble();
		node->Rotation.y = rotationArray.Get(1).AsDouble();
		node->Rotation.z = rotationArray.Get(2).AsDouble();
		node->Rotation.w = rotationArray.Get(3).AsDouble();
	}

	if(glbNode.Contains("scale")) {
		PJL::JSONArray scaleArray = glbNode.Get("scale").AsArray();
		node->Scale.x = scaleArray.Get(0).AsDouble();
		node->Scale.y = scaleArray.Get(1).AsDouble();
		node->Scale.z = scaleArray.Get(2).AsDouble();
	}

	if(glbNode.Contains("translation")) {
		PJL::JSONArray translationArray = glbNode.Get("translation").AsArray();
		node->Translation.x = translationArray.Get(0).AsDouble();
		node->Translation.y = translationArray.Get(1).AsDouble();
		node->Translation.z = translationArray.Get(2).AsDouble();
	}

	if(!glbNode.Contains("mesh")) {
		return node;
	}

	node->Mesh = glbNode.Get("mesh").AsInt();

	if(!glbNode.Contains("skin")) {
		return node;
	}

	node->Skin = glbNode.Get("skin").AsInt();

	return node;
}

GLBModelMesh* GLBFileLoader::LoadMeshFromGLBFile(int meshIndex, const PJL::JSONObject& glbHeader, const std::vector<uint8_t>& binaryData) {
	const PJL::JSONArray& glbMeshes = glbHeader.Get("meshes").AsArray();

	const auto& glbMesh = glbMeshes.Get(meshIndex);

	GLBModelMesh* glbM = new GLBModelMesh();

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
			glbM->MaterialIndex = glbPrimitive.Get("material").AsInt();
		}

		// Interleave vertex data
		size_t vertexCount = positions.size();
		glbM->Vertices.resize(vertexCount);
		for (size_t i = 0; i < vertexCount; ++i) {
			glbM->Vertices[i].Position = positions[i];
			if (!normals.empty()) {
				glbM->Vertices[i].Normals = normals[i];
			}
			if (!texCoords.empty()) {
				glbM->Vertices[i].TextureCoordinates = texCoords[i];
			}

			if(!joints.empty()) {
				glbM->Vertices[i].Joints = joints[i];
			} else {
				glbM->Vertices[i].Joints = {0, 0, 0, 0};
			}

			if(!joints.empty()) {
				glbM->Vertices[i].Weights = weights[i];
			} else {
				glbM->Vertices[i].Weights = {0, 0, 0, 0};
			}
		}

		if (glbPrimitive.Contains("indices")) {
			int accessorIndex = glbPrimitive.Get("indices").AsInt();
			// Load indices using the accessorIndex...
			LoadIndexData(glbHeader, binaryData, accessorIndex, glbM->Indices);
		}
	}

	return glbM;
}


GLBModelSkin* GLBFileLoader::LoadSkinFromGLBFile(int skinIndex, const PJL::JSONObject& glbHeader, const std::vector<uint8_t>& binaryData) {
	GLBModelSkin* skin = new GLBModelSkin();

	PJL::JSONObject glbSkin = glbHeader.Get("skins").AsArray().Get(skinIndex).AsObject();

	PJL::JSONArray glbJoints = glbSkin.Get("joints").AsArray();
	for(int i = 0; i < glbJoints.Size(); i++) {
		skin->Joints.push_back(glbJoints.Get(i).AsInt());
	}

	if(!glbSkin.Contains("inverseBindMatrices")) {
		return skin;
	}
		
	int glbBindMatricesIndex = glbSkin.Get("inverseBindMatrices").AsInt();

	PJL::JSONValue val = glbHeader.Get("inverseBindMatrices");
	LoadAttributeData(glbHeader, binaryData, glbBindMatricesIndex, skin->InverseBindMatrices);

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
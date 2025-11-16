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
		const cgltf_accessor* pWeightsAccessor = nullptr;
		const cgltf_accessor* pJointsAccessor = nullptr;

		for (size_t j = 0; j < prim.attributes_count; ++j) {
			const cgltf_attribute& attr = prim.attributes[j];
			if (attr.type == cgltf_attribute_type_position)
				pPosAccessor = attr.data;
			else if (attr.type == cgltf_attribute_type_normal)
				pNormAccessor = attr.data;
			else if (attr.type == cgltf_attribute_type_texcoord)
				pUVAccessor = attr.data;
			else if (attr.type == cgltf_attribute_type_weights)
				pWeightsAccessor = attr.data;
			else if(attr.type == cgltf_attribute_type_joints)
				pJointsAccessor = attr.data;
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

			if (pWeightsAccessor) {
				float weights[4] = {};
				cgltf_accessor_read_float(pWeightsAccessor, v, weights, 4);
				vert.Weights = Vector4(weights[0], weights[1], weights[2], weights[3]);
			}

			if(pJointsAccessor) {
				unsigned int joints[4] = {};
				cgltf_accessor_read_uint(pJointsAccessor, v, joints, 4);
				// TODO static cast
				vert.Joints = UBYTE4{
					static_cast<unsigned char>(joints[0]),
					static_cast<unsigned char>(joints[1]),
					static_cast<unsigned char>(joints[2]),
					static_cast<unsigned char>(joints[3])
				};
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
		// mySkin.Name = skin.name;

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
			std::vector<float> times;
			for(int ac = 0; ac < pTimeAccessor->count; ac++) {
				float f;
				cgltf_accessor_read_float(pTimeAccessor, ac, &f, 1);
				times.push_back(f);
			}

			// Load values
			const cgltf_accessor* pValueAccessor = pSampler->output;
			size_t valueCount = pValueAccessor->count;
			size_t components = cgltf_num_components(pValueAccessor->type);

			if(valueCount > times.size()) {
				throw std::runtime_error("kaputtski");
			}

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

#pragma once

#include <GLBFile.h>
#include <common/PMG_Common.h>
#include <common/PloinkysJSONLibrary.h>

class GLBFileLoader {
public:
	static GLBModel* LoadUsingLib(std::string modelName, AssetManager* assetManager);
	static GLBModel* LoadModelFromGLBFile(std::string modelName, AssetManager* assetManager);
	static GLBNode* LoadNodeFromGLBFile(int nodeIndex, const PJL::JSONObject& glbHeader, const std::vector<uint8_t>& binaryData);
	static GLBModelMesh* LoadMeshFromGLBFile(int meshIndex, const PJL::JSONObject& glbHeader, const std::vector<uint8_t>& binaryData);
	static GLBModelSkin* LoadSkinFromGLBFile(int skinIndex, const PJL::JSONObject& glbHeader, const std::vector<uint8_t>& binaryData);

	template <typename T>
	static void LoadAttributeData(const PJL::JSONObject& gltfJson, const std::vector<uint8_t>& binaryData, int accessorIndex, std::vector<T>& attributeData);
	static void LoadIndexData(const PJL::JSONObject& gltfJson, const std::vector<uint8_t>& binaryData, int accessorIndex, std::vector<uint32_t>& indices);
	static void LoadImageData(const PJL::JSONObject& gltfJson, const std::vector<uint8_t>& binaryData, int imageIndex, std::vector<uint8_t>& imageData);
};

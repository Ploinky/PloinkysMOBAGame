#pragma once

#include <Common/PMG_Common.h>
#include <Common/PloinkysJSONLibrary.h>
#include <DirectXMath.h>
#include <ModelNode.h>
#include <Material.h>
#include <Animation.h>
#include <Armature.h>
#include <map>
#include <string.h>
#include <Mesh.h>

class Model {
public:
	~Model();

	std::map<int, ModelNode*> Nodes;
	std::map<int, Mesh*> Meshes;
	std::map<int, Material*> Materials;
	std::map<int, Armature*> Skins;
	std::map<std::string, Animation*> Animations;
};

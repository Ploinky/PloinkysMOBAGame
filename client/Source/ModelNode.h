#pragma once

#include <vector>
#include <Mesh.h>
#include <Material.h>
#include <Armature.h>

class ModelNode {
public:
	int Mesh;
	int Skin;
	std::vector<int> Children;
	int parent;
};

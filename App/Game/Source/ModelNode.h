#pragma once

#include <vector>
#include <Mesh.h>
#include <Material.h>
#include <Armature.h>

namespace PMG {
	class ModelNode {
	public:
		int Mesh;
		int Skin;
		std::vector<int> Children;
		int parent;
	};
}
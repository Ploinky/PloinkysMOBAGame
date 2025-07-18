#include <Model.h>

Model::~Model() {
	for (auto node_it: Nodes) {
		delete node_it.second;
	}
	for (auto mesh_it: Meshes) {
		delete mesh_it.second;
	}
	for (auto material_it : Materials) {
		delete material_it.second;
	}
	for (auto skin_it : Skins) {
		delete skin_it.second;
	}
	for (auto anim_it : Animations) {
		delete anim_it.second;
	}
}

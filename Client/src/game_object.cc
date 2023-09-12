#include "game_object.h"
#include "mesh.h"

namespace PMG {
	GameObject::~GameObject() {
		if (mesh_component != nullptr) {
			delete mesh_component;
			mesh_component = nullptr;
		}
	}

	void GameObject::Update(double dt) {
		if (mesh_component != nullptr) {
			mesh_component->position = position;
			mesh_component->rotation = rotation;
			mesh_component->Update(dt);
		}
	}
}
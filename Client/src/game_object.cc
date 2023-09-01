#include "game_object.h"

namespace PMG {
	GameObject::~GameObject() {
		if (mesh != nullptr) {
			delete mesh;
		}
	}
}
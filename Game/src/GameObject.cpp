#include "GameObject.h"
#include "Mesh.h"
#include "Renderer.h"

namespace PMG {
	GameObject::~GameObject() {
		if (renderable != nullptr) {
			delete renderable;
			renderable = nullptr;
		}
	}

	void GameObject::Update(double dt) {
		if (renderable != nullptr) {
			renderable->position = position;
			renderable->rotation = rotation;
			renderable->Update(dt);
		}
	}

	void GameObject::Render(Renderer* renderer) {
		if (renderable != nullptr) {
			renderable->Render(renderer);
		}
	}
}
#include "game_object.h"
#include "mesh.h"
#include "renderer.h"

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
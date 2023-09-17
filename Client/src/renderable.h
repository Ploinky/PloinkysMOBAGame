#pragma once

#include "pmg_physics.h"
#include <string>

namespace PMG {
	class Renderer;

	class IRenderable {
	public:
		virtual ~IRenderable() {};

		virtual void Render(Renderer* renderer) = 0;
		virtual void Update(double dt) = 0;
		virtual void PlayAnimation(std::string animation) = 0;

		Physics::Vector3 position;
		Physics::Vector3 rotation;
	};
}
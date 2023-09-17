#pragma once

#include "pmg_physics.h"
#include "pmg_types.h"
#include "renderable.h"

#include <string>

namespace PMG {
	class Mesh;
	class Renderer;

	class GameObject {
	public:
		virtual ~GameObject();

		virtual void Update(double dt);
		virtual void Render(Renderer* renderer);

		UnitId unit_id;
		unsigned int health;
		unsigned int max_health;

		Physics::Vector3 position;
		Physics::Vector3 rotation;
		unsigned long long position_received;

		bool has_healthbar = true;
		bool has_title = true;

		Team team;

		IRenderable* renderable = nullptr;

		bool destroy = false;
	};
}
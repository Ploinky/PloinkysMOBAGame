#pragma once

#include "mesh.h"
#include "pmg_physics.h"
#include "pmg_types.h"

namespace PMG {
	class GameObject {
	public:
		~GameObject();

		UnitId unit_id;
		unsigned int health;
		unsigned int max_health;

		Physics::Vector3 position;
		Physics::Vector3 rotation;
		unsigned long long position_received;

		bool has_healthbar = true;
		bool has_title = true;

		Team team;

		std::string mesh;
	};
}
#pragma once

#include "pmg_physics.h"

namespace PMG {
	class Particle {
	public:
		Physics::Vector3 position;
		double color[4];
		Physics::Vector3 velocity;
		double lifetime;
	};
}
#pragma once

#include "common/pmg_physics.h"

class Particle {
public:
	Vector3 position;
	double color[4];
	Vector3 velocity;
	double lifetime;
};

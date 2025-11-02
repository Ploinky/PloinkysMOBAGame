#pragma once

#include <common/PMG_Common.h>

class Missile {
public:
	UnitId owner;
	UnitId target;
	Vector3 target_point;
	Team team;

	Vector3 position;
	Vector3 rotation;

	unsigned int damage;
	unsigned int missile_speed;
	int max_distance;
};
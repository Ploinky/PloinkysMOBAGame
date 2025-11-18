#pragma once

#include "common/pmg_physics.h"
#include "common/pmg_types.h"
#include <string>

class Mesh;
class CRenderer;


class GameObject {
public:
	virtual ~GameObject();

	virtual void Update(float dt);

	UnitId unit_id;

	Vector3 position;

	Vector3 rotation;
	unsigned long long position_received;

	bool has_healthbar = true;
	bool has_title = true;

	Team team;

	bool destroy = false;

	bool dead = false;
	bool bIsCasting = false;
	bool bIsAttacking = false;

	std::string uPrefab = "";
};

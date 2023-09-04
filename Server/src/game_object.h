#pragma once

#include "pmg_physics.h"

namespace PMG {
	class Game;

	class GameObject {
	public:
		unsigned int unit_id;
		bool is_destroyed = false;

		virtual void Think(float dt, Game* game) = 0;
		virtual bool IsTargetable() { return false; };
		virtual Physics::Sphere GetHitbox() { return Physics::Sphere(); };
	};
}
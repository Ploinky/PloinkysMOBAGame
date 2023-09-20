#pragma once

#include <vector>
#include "pmg_types.h"

namespace PMG {
	class Game;

	class IGameObject {
	public:
		// Prepare object for frame (buffs, cc, ...)
		virtual void Update(Game* game, float dt) = 0;

		// Do something?
		virtual void Act(Game* game, float dt) {}; // TODO pure virtual

		// Synchronize with Clients...
		virtual void Sync(std::vector<uint8_t>* data) {}; // TODO pure virtual

		// Something happened?!
		virtual void OnCollision(Game* game, IGameObject* other) = 0;

		UnitId unit_id;
		double collision_radius = 0;
		bool is_destroyed = false;
	};
}
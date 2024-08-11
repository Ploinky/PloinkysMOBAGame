#pragma once

#include "Attackable.h"
#include "Common/pmg_physics.h"
#include <vector>

namespace PMG {

	extern AttackableStats minion_attackable_stats;

	class Minion : public Attackable {
	public:
		Minion(std::vector<Physics::Vector3> waypoints) : waypoints_(waypoints), Attackable(minion_attackable_stats) {
			collision_radius = 50.0f;
		};

		virtual void Update(Client* game, float dt) override;
	
	private:
		std::vector<Physics::Vector3> waypoints_;
	};
}
#include "minion.h"

namespace PMG {
	AttackableStats minion_attackable_stats = {
		10,
		10,
		0,
		1,
		0,
	};

	void Minion::Update(Game* game, float dt) {
		MoveToward(waypoints_[0].x, waypoints_[0].z, game, 3);
	};
}
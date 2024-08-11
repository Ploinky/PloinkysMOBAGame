#include "Minion.h"
#include "Game.h"
#include "Missile.h"
#include "IGameObject.h"

namespace PMG {
	AttackableStats minion_attackable_stats = {
        300, // move_speed
		10,
		10,
		0,
		1,
		0,
	};

	void Minion::Update(Client* game, float dt) {
        Attackable::Update(game, dt);

		if(current_action_ == nullptr || current_action_->type == GameObjectActionType::STOP) {
			// if no action -> continue to next waypoint
			// TODO
			DoAction(new GameObjectActionAttackMove(this->unit_id, waypoints_[0], game));
		}
	};
}
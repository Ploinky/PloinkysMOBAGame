#include "building.h"
#include "game.h"

namespace PMG {
	void TowerGameObjectController::Think(Game* game, GameObject* go) {
		if (go->current_action != nullptr && go->current_action->type == GameObjectActionType::ATTACK_UNIT) {
			GameObjectActionAttackUnit* attack = (GameObjectActionAttackUnit*)go->current_action;

			double dist = (game->GetGameObjectById(attack->target_net_id)->position - go->position).Length();

			if (dist > 5) {
				delete go->current_action;
				go->current_action = new GameObjectActionStop();
			}

			return;
		}

		// try to find unit to attack
		for (auto go_it : game->game_objects_) {
			GameObject* other_go = go_it.second;

			if (other_go == nullptr || other_go->unit_id == go->unit_id) {
				continue;
			}

			if (!other_go->IsTargetable()) {
				continue;
			}

			double dist = (other_go->position - go->position).Length();

			if (dist <= 5) {
				go->current_action = new GameObjectActionAttackUnit(other_go->unit_id);
			}
		}
	}

	Building::Building() {
		target_type = TargetType::BUILDING;
		controller = new TowerGameObjectController();
	}
}
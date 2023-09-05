#include "building.h"
#include "game.h"

namespace PMG {
	void Building::Think(float dt, Game* game) {
		for (auto go_it : game->game_objects_) {
			GameObject* go = go_it.second;

			if (go == nullptr || go->unit_id == this->unit_id) {
				continue;
			}

			if (!go->IsTargetable()) {
				continue;
			}

			Targetable* character = (Targetable*)go;
			double dist = (character->position - this->position).Length();
			if (dist < 5) {
				game->ApplyDamage(character, 10);
			}
		}
	}
}
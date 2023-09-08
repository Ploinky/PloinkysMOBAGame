#include "area_spell.h"
#include "game.h"

namespace PMG {
	AreaSpell::AreaSpell(AreaSpellData data, SpellTargetInfo* target_info) : spell_data(data), target_info(target_info) {
		collision_radius = spell_data.radius;
		target_type = TargetType::UNTARGETABLE;
	}

	void AreaSpell::OnCollision(Game* game, GameObject* target) {
		if (!spell_data.can_hit_allies && target->team == owner->team) {
			return;
		}

		if (!spell_data.can_hit_buildings && target->target_type == TargetType::BUILDING) {
			return;
		}

		objects_to_hit.insert(target);
	}

	void AreaSpell::Update(float dt, Game* game) {
		double ms = 1000.0 * dt;

		time_since_tick_ += ms;
		time_since_cast_ += ms;

		if (time_since_tick_ >= (1000.0 / spell_data.tickrate)) {
			// now we hit!
			for (GameObject* go : objects_to_hit) {
				TargetHit(game, owner, go);
			}
			time_since_tick_ -= (1000.0 / spell_data.tickrate);
		}

		// check if we need to stop
		if (time_since_cast_ >= spell_data.duration) {
			game->DestroyGameObject(this);
			return;
		}

		// do not forget to clear so we do not hit objects that are no longer colliding!
		objects_to_hit.clear();
	}
}
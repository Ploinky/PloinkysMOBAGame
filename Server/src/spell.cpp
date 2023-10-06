
#include "spell.h"
#include "game.h"
#include "missile.h"
#include "attackable.h"

namespace PMG {
	void Spell::CastStart(Client* game, Attackable* spell_owner, SpellTargetInfo* target_info) {
		Physics::Vector3 target_point;

		if (target_info->target == nullptr) {
			// no target, but targeted at a point, so turn to point
			target_point = target_info->target_point;
		}
		else {
			if (target_info->target->unit_id == spell_owner->unit_id) {
				// self cast, do not turn!
				return;
			}
			// turn towards target unit
			target_point = target_info->target->position;
		}

		spell_owner->rotation.y = atan2(target_point.x - spell_owner->position.x, target_point.z - spell_owner->position.z) * 180.0f / M_PI;
	}

	void Spell::Cast(Client* game, Attackable* spell_owner, SpellTargetInfo* target_info) {
		OnCast(game, spell_owner, target_info);
	}
}
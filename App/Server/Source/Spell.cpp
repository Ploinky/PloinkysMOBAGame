
#include "Spell.h"
#include "Game.h"
#include "Missile.h"
#include "Attackable.h"

namespace PMG {
	void Spell::CastStart(Attackable* spell_owner, SpellTargetInfo* target_info) {
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

		spell_owner->rotation.y = Physics::CalculateAngle({spell_owner->position.x, spell_owner->position.z}, {target_point.x, target_point.z});
	}

	void Spell::Cast(Client* game, Attackable* spell_owner, SpellTargetInfo* target_info) {
		OnCast(game, spell_owner, target_info);
	}
}
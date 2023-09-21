
#include "spell.h"
#include "game.h"
#include "missile.h"
#include "attackable.h"

namespace PMG {
	void Spell::CastStart(Game* game, Attackable* spell_owner, SpellTargetInfo* target_info) {
		Physics::Vector3 target_point;
		if (target_info->target != nullptr) {
			target_point = target_info->target->position;
		}
		else {
			target_point = target_info->target_point;
		}

		spell_owner->rotation.y = atan2(target_point.x - spell_owner->position.x, target_point.z - spell_owner->position.z) * 180.0f / M_PI;
	}

	void Spell::Cast(Game* game, Attackable* spell_owner, SpellTargetInfo* target_info) {
		OnCast(game, spell_owner, target_info);
	}
}
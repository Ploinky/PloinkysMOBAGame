
#include "spell.h"
#include "game.h"
#include "missile.h"

namespace PMG {
	void Spell::CastStart(Game* game, GameObject* spell_owner, SpellTargetInfo* target_info) {
		Physics::Vector3 target_point;
		if (target_info->target != nullptr) {
			target_point = target_info->target->position;
		}
		else {
			target_point = target_info->target_point;
		}

		spell_owner->rotation.y = atan2(target_point.x - spell_owner->position.x, target_point.z - spell_owner->position.z) * 180.0f / M_PI;

		Networking::UnitMovePacket* move = new Networking::UnitMovePacket();
		move->unit = spell_owner->unit_id;
		move->x = spell_owner->position.x;
		move->y = spell_owner->position.y;
		move->z = spell_owner->position.z;
		move->r = spell_owner->rotation.y;
		game->SendPacket(move);
	}

	void Spell::Cast(Game* game, GameObject* spell_owner, SpellTargetInfo* target_info) {
		OnCast(game, spell_owner, target_info);
	}
}
#include "missile_spell.h"
#include "game.h"
#include "spell.h"

namespace PMG {
	MissileSpell::MissileSpell(MissileSpellData data, GameObject* owner, SpellTargetInfo* target_info) : spell_data(data), target_info(target_info) {
		collision_radius = data.radius;
		position = owner->position;
		origin = owner->position;
		target_type = TargetType::UNTARGETABLE;

		if (spell_data.distance != -1) {
			target_info->target_point = position + ((target_info->target_point - position).Normalize() * spell_data.distance);
		}
	}
	
	void MissileSpell::OnCollision(Game* game, GameObject* target) {
		if (target_info->target != nullptr && target_info->target->unit_id != target->unit_id) {
			return;
		}

		if (!spell_data.can_hit_self && target->unit_id == owner->unit_id) {
			return;
		}

		if (!spell_data.can_hit_allies && target->team == owner->team) {
			return;
		}

		if (!spell_data.can_hit_buildings && target->target_type == TargetType::BUILDING) {
			return;
		}

		TargetHit(game, owner, target);

		// TODO this does not work
		if (spell_data.max_targets_hit == 1) {
			game->DestroyGameObject(this);
		}
	}

	void MissileSpell::Update(float dt, Game* game) {
        Physics::Vector3 current_position = position;

        Physics::Vector3 target_current_position = target_info->target_point;

        if (target != nullptr) {
            target_current_position = this->target->position;
        }

		if (spell_data.distance != -1 && (position -origin).Length() >= spell_data.distance) {
			game->DestroyGameObject(this);
			return;
        }

		Physics::Vector3 direction_vector = target_current_position - current_position;
        Physics::Vector3 scaled = direction_vector.ScaleToLength(spell_data.speed);
        scaled = scaled * dt;

        position = position + scaled;

        double rotationY = -atan2(target_current_position.z - position.z, target_current_position.x - position.x) * 180.0f / M_PI;

		Networking::UnitMovePacket* move = new Networking::UnitMovePacket();
		move->unit = unit_id;
		move->x = position.x;
		move->y = position.y;
		move->z = position.z;
		move->r = rotationY;
        game->SendPacket(move);
        return;
	}
}
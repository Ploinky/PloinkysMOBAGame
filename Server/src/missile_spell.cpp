#include "missile_spell.h"
#include "game.h"
#include "spell.h"
#include "attackable.h"

namespace PMG {
	MissileSpell::MissileSpell(MissileSpellData data, Attackable* owner, SpellTargetInfo* target_info) : spell_data(data), target_info(target_info) {
		collision_radius = data.radius;
		position = owner->position;
		origin = owner->position;

		if (spell_data.distance != -1) {
			target_info->target_point = position + ((target_info->target_point - position).Normalize() * spell_data.distance);
		}
	}
	
	void MissileSpell::OnCollision(Game* game, IGameObject* t) {
		Attackable* target = dynamic_cast<Attackable*>(t);

		if (target == nullptr) {
			return;
		}

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
			is_destroyed = true;
		}
	}

	void MissileSpell::Update(Game* game, float dt) {
        Physics::Vector3 current_position = position;

        Physics::Vector3 target_current_position = target_info->target_point;

        if (target != nullptr) {
            target_current_position = this->target->position;
        }

		if (spell_data.distance != -1 && (position -origin).Length() >= spell_data.distance) {
			is_destroyed = true;
			return;
        }

		Physics::Vector3 direction_vector = target_current_position - current_position;
        Physics::Vector3 scaled = direction_vector.ScaleToLength(spell_data.speed);
        scaled = scaled * dt;

        position = position + scaled;

        double rotationY = -atan2(target_current_position.z - position.z, target_current_position.x - position.x) * 180.0f / M_PI;
		rotation.y = rotationY;
	}

	void MissileSpell::Sync(std::vector<uint8_t>* data) {
		if (!is_destroyed && !spawn_synced) {
			Networking::SpawnPacket pck = Networking::SpawnPacket();
			pck.unit = unit_id;
			pck.team = team;
			pck.unit_type = UnitPrefab::THROW_FOOTBALL;
			pck.x = position.x;
			pck.y = position.z;

			pck.Write(data);
			spawn_synced = true;
		}

		if (is_destroyed && spawn_synced) {
			Networking::DespawnPacket pck = Networking::DespawnPacket();
			pck.unit = unit_id;

			pck.Write(data);
			spawn_synced = false;
		}

		Networking::UnitMovePacket* move = new Networking::UnitMovePacket();
		move->unit = unit_id;
		move->x = position.x;
		move->y = position.y;
		move->z = position.z;
		move->r = rotation.y;
		move->Write(data);
	};
}
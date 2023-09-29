#include "area_spell.h"
#include "game.h"

namespace PMG {
	AreaSpell::AreaSpell(AreaSpellData data, SpellTargetInfo* target_info) : spell_data(data), target_info(target_info) {
		collision_radius = spell_data.radius;
	}

	void AreaSpell::OnCollision(Game* game, IGameObject* t) {
		Attackable* target = dynamic_cast<Attackable*>(t);

		if (target == nullptr) {
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

		objects_to_hit.insert(target);
	}

	void AreaSpell::Update(Game* game, float dt) {
		double ms = 1000.0 * dt;

		time_since_tick_ += ms;
		time_since_cast_ += ms;

		if (time_since_tick_ >= (1000.0 / spell_data.tickrate)) {
			// now we hit!
			for (Attackable* go : objects_to_hit) {
				TargetHit(game, owner, go);
			}
			time_since_tick_ -= (1000.0 / spell_data.tickrate);
		}

		// check if we need to stop
		if (time_since_cast_ >= spell_data.duration) {
			is_destroyed = true;
			return;
		}

		// do not forget to clear so we do not hit objects that are no longer colliding!
		objects_to_hit.clear();
	}

	void AreaSpell::Sync(std::vector<uint8_t>* data) {
		if (!spawn_synced && !is_destroyed) {
			Networking::SpawnPacket pck = Networking::SpawnPacket();
			pck.unit = unit_id;
			pck.team = owner->team;
			pck.unit_type = UnitPrefab::GENERIC_EMPTY;
			pck.x = position.x;
			pck.y = 0.1f;
			pck.z = position.z;

			pck.Write(data);
			spawn_synced = true;
		}

		if (spawn_synced && is_destroyed) {
			Networking::DespawnPacket pck = Networking::DespawnPacket();
			pck.unit = unit_id;

			pck.Write(data);
			spawn_synced = false;
		}

		Networking::UnitMovePacket* move = new Networking::UnitMovePacket();
		move->unit = unit_id;
		move->x = position.x;
		move->y = 0.1f;
		move->z = position.z;
		move->r = rotation.y;
		move->Write(data);
	}
}
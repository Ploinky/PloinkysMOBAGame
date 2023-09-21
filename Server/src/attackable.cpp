#include "attackable.h"
#include "pmg_networking.h"

namespace PMG {
	void Attackable::Update(float dt) {
		if (stats.health == stats.max_health) {
			return;
		}
		
		stats.health += stats.health_regen * (dt / (1000.0f / 60.0f));

		if (stats.health > stats.max_health) {
			stats.health = stats.max_health;
		}

		stats_updated = true;
	}

	void Attackable::TakeDamage(float damage, IGameObject* source) {
		stats.health -= damage;

		stats_updated = true;
	}

	void Attackable::Heal(float heal, IGameObject* source) {
		if (stats.health = stats.max_health) {
			return;
		}

		stats.health += heal;

		if (stats.health > stats.max_health) {
			stats.health = stats.max_health;
		}

		stats_updated = true;
	}

	void Attackable::Sync(std::vector<uint8_t>* data) {
		if (!spawn_synced) {
			Networking::SpawnPacket pck = Networking::SpawnPacket();
			pck.unit = unit_id;
			pck.team = team;
			pck.unit_type = prefab;
			pck.x = position.x;
			pck.y = position.y;

			pck.Write(data);
			spawn_synced = true;
		}

		if (stats_updated) {
			Networking::UnitStatsPacket pck = Networking::UnitStatsPacket();
			pck.unit = unit_id;
			pck.health = stats.health;
			pck.max_health = stats.max_health;

			pck.Write(data);

			stats_updated = false;
		}

		if (stats_updated) {
			Networking::UnitStatsPacket pck = Networking::UnitStatsPacket();
			pck.unit = unit_id;
			pck.health = stats.health;
			pck.max_health = stats.max_health;

			pck.Write(data);

			stats_updated = false;
		}

		if (new_animation.length() > 0) {
			Networking::AnimationPacket* pck = new Networking::AnimationPacket();
			pck->unit_id = unit_id;
			pck->animation_name = new_animation;
			pck->Write(data);

			new_animation = "";
		}

		Networking::UnitMovePacket* move = new Networking::UnitMovePacket();
		move->unit = unit_id;
		move->x = position.x;
		move->y = position.y;
		move->z = position.z;
		move->r = rotation.y;
		move->Write(data);
	}
}
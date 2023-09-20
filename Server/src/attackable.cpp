#include "attackable.h"
#include "pmg_networking.h"

namespace PMG {
	void Attackable::Update(float dt) {
		stats.health += stats.health_regen * (dt / (1000.0f / 60.0f));

		stats_updated = true;
	}

	void Attackable::TakeDamage(float damage, IGameObject* source) {
		stats.health -= damage;

		stats_updated = true;
	}

	void Attackable::Heal(float heal, IGameObject* source) {
		stats.health += heal;

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
			return; // TODO nope
		}

		if (stats_updated) {
			Networking::UnitStatsPacket pck = Networking::UnitStatsPacket();
			pck.unit = unit_id;
			pck.health = stats.health;
			pck.max_health = stats.max_health;

			pck.Write(data);

			stats_updated = false;
		}
	}
}
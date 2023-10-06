#include "minion_spawner.h"
#include "game.h"
#include "logger.h"
#include "minion.h"

namespace PMG {
	void MinionSpawner::Update(Client* game, float dt) {
		int ticks_since = game->gameTick - last_spawn_tick;

		// wait 30 seconds...
		if (ticks_since < 60 * 5) {
			return;
		}

		// spawn!
		Logger::Msg("Spawn new minions!");

		Minion* minion = new Minion(waypoints_);
		minion->position = position;
		minion->prefab = UnitPrefab::MINION;
		minion->team = team;
		game->AddGameObject(minion);

		last_spawn_tick = game->gameTick;
	}

	void MinionSpawner::OnCollision(Client* game, IGameObject* other) {
		// do nothing!
	};
}
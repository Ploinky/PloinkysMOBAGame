#include "minion_spawner.h"
#include "game.h"
#include "logger.h"

namespace PMG {
	void MinionSpawner::Update(Game* game, double dt) {
		int ticks_since = game->gameTick - last_spawn_tick;

		// wait 30 seconds...
		if (ticks_since < 60 * 30) {
			return;
		}

		// spawn!
		Logger::Msg("Spawn new minions!");

		last_spawn_tick = game->gameTick;
	}

	void MinionSpawner::OnCollision(Game* game, IGameObject* other) {
		// do nothing!
	};
}
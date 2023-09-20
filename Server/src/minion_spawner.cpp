#include "minion_spawner.h"
#include "game.h"
#include "logger.h"
#include "attackable.h"

namespace PMG {
	void MinionSpawner::Update(Game* game, float dt) {
		int ticks_since = game->gameTick - last_spawn_tick;

		// wait 30 seconds...
		if (ticks_since < 60 * 30) {
			return;
		}

		// spawn!
		Logger::Msg("Spawn new minions!");

		AttackableStats att_stats{};
		att_stats.health = 10;
		att_stats.max_health = 10;
		att_stats.level = 0;
		att_stats.health_regen = 1;
		att_stats.experience = 0;
		Attackable* att = new Attackable(att_stats);
		game->AddGameObject(att);

		last_spawn_tick = game->gameTick;
	}

	void MinionSpawner::OnCollision(Game* game, IGameObject* other) {
		// do nothing!
	};
}
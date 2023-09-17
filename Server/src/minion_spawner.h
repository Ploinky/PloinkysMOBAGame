#pragma once

#include "igame_object.h"
#include "game_object.h"

namespace PMG {
	class Game;

	class MinionSpawner : public IGameObject {
	public:
		virtual void Update(Game* game, double dt) override;
		virtual void OnCollision(Game* game, IGameObject* other) override;

	private:
		Team team;
		double last_spawn_tick = 0;
	};
}
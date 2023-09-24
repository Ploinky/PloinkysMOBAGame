#pragma once

#include "igame_object.h"
#include "game_object.h"

namespace PMG {
	class Game;

	class MinionSpawner : public IGameObject {
	public:
		MinionSpawner(std::vector<Physics::Vector3> waypoints) : waypoints_(waypoints) {};
		virtual void Update(Game* game, float dt) override;
		virtual void OnCollision(Game* game, IGameObject* other) override;

		Team team;
	private:
		double last_spawn_tick = 0;
		std::vector<Physics::Vector3> waypoints_;
	};
}
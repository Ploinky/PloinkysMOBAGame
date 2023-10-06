#pragma once

#include "igame_object.h"
#include "game_object.h"

namespace PMG {
	class Client;

	class MinionSpawner : public IGameObject {
	public:
		MinionSpawner(std::vector<Physics::Vector3> waypoints) : waypoints_(waypoints) {};
		virtual void Update(Client* game, float dt) override;
		virtual void OnCollision(Client* game, IGameObject* other) override;

		Team team;
	private:
		double last_spawn_tick = 0;
		std::vector<Physics::Vector3> waypoints_;
	};
}
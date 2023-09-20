#pragma once

#include "attackable.h"
#include "game_object.h"

namespace PMG {
	class Game;

	class Building : public Attackable {
	public:
		Building(Team team);

		virtual void Update(Game* game, float dt) {};
		virtual void Act(Game* game, float dt) override;
		virtual Physics::Sphere GetHitbox() { return Physics::Sphere(position, 1); };
	};
}
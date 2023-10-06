#pragma once

#include "attackable.h"
#include "game_object.h"

namespace PMG {
	class Client;

	class Building : public Attackable {
	public:
		Building(Team team);

		virtual void Update(Client* game, float dt) {};
		virtual void Act(Client* game, float dt) override;
		virtual Physics::Sphere GetHitbox() { return Physics::Sphere(position, 1); };
	};
}
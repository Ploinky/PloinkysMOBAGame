#pragma once

#include "Attackable.h"
#include "GameObject.h"

namespace PMG {
	class Client;

	class Building : public Attackable {
	public:
		Building(Team team);

		virtual void Update(Client* game, float dt) {};
		virtual Physics::Sphere GetHitbox() { return Physics::Sphere(position, 1); };
	};
}
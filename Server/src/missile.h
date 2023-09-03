#pragma once

#include "game_object.h"
#include "pmg_types.h"
#include "character.h"
#include "game.h"
namespace PMG {
	class Missile : public GameObject {
	public:
		void Think(float dt, Game* game);

		GameObject* owner;
		Character* target;
		Team team;

		Physics::Vector3 position;
		Physics::Vector3 rotation;

		unsigned int damage;
		unsigned int missile_speed;
	};
}
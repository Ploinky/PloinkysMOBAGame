#pragma once

#include "game_object.h"
#include "pmg_types.h"
#include "character.h"
#include "game.h"
namespace PMG {
	class Missile : public GameObject {
	public:
		virtual void Think(float dt, Game* game);
		virtual Physics::Sphere GetHitbox() { return Physics::Sphere(position, 0.1); }
		GameObject* owner;
		Character* target;
		Physics::Vector3 target_point;
		Team team;

		Physics::Vector3 position;
		Physics::Vector3 rotation;

		unsigned int damage;
		unsigned int missile_speed;
		int max_distance;
	};
}
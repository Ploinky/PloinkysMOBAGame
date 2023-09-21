#pragma once

#include "game_object.h"
#include "pmg_types.h"
#include "attackable.h"
#include "game.h"
namespace PMG {
	class Missile : public IGameObject {
	public:
		virtual void Update(Game* game, float dt) {};
		virtual void Act(Game* game, float dt);
		virtual void OnCollision(Game* game, IGameObject* other) {};
		virtual void Sync(std::vector<uint8_t>* data) override;
		virtual Physics::Sphere GetHitbox() { return Physics::Sphere(position, 0.1); }
		Attackable* owner;
		Attackable* target;
		Physics::Vector3 target_point;
		Team team;

		Physics::Vector3 position;
		Physics::Vector3 rotation;

		unsigned int damage;
		unsigned int missile_speed;
		int max_distance;

		bool spawn_synced = false;
	};
}
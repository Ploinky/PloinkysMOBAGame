#pragma once

#include "GameObject.h"
#include "Common/pmg_types.h"
#include "Attackable.h"
#include "Game.h"
namespace PMG {
	class Missile : public IGameObject {
	public:
		virtual void Update(Client* game, float dt) {};
		virtual void Act(Client* game, float dt);
		virtual void OnCollision(Client* game, IGameObject* other) {};
		virtual void Sync(std::vector<uint8_t>* data) override;
		virtual Physics::Sphere GetHitbox() { return Physics::Sphere(position, 0.1f); }
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
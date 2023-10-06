#pragma once

#include "pmg_types.h"
#include "igame_object.h"
#include "pmg_physics.h"
#include "game_object.h"

namespace PMG {

	class AttackableStats {
	public:
		int move_speed;

		int health;
		int max_health;

		int experience;
		int level;

		int health_regen;
	};

	enum class TargetType {
		CHARACTER,
		BUILDING,
		UNTARGETABLE
	};

	// Attackable can units have stats, levels, items, buffs etc.
	// Also respawns etc
	class Attackable : public IGameObject {
	public:
		// Constructor initializes important stats
		Attackable(AttackableStats stats) : stats(stats) {};

		virtual void Update(Client* game, float dt) {
			// TODO not so elegant
			Update(dt);
		};
		void Update(float dt);
		virtual void OnCollision(Client* game, IGameObject* other) override;
		virtual void Sync(std::vector<uint8_t>* data) override;
		void MoveToward(double x, double z, Client* game);

		void TakeDamage(float damage, IGameObject* source);
		void Heal(float heal, IGameObject* source);

		virtual void Die();

		GameObjectAction* current_action_ = nullptr;

		std::string new_animation;

		AttackableStats stats;
		AttackableStats modifiers;

		TargetType target_type;

		unsigned long prefab = UnitPrefab::GENERIC_EMPTY;

		Team team;

		basic_attack_info_t basic_attack_info = {
			BasicAttackType::MELEE, // type
			1, // range
			1, // damage
			1, // attack speed
			0.25, // hit point
			0 // last attack
		};
		std::vector<Buff*> buffs;
		nav_agent_t nav_agent;

	protected:
		// true -> initial spawn has been synced
		bool spawn_synced = false;

		// true -> stats need to be synced
		bool stats_updated = true;
	};
}
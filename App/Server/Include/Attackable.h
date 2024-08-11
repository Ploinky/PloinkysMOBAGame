#pragma once

#include "Common/pmg_types.h"
#include "IGameObject.h"
#include "Common/pmg_physics.h"
#include "GameObject.h"
#include <cmath>

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
		Attackable(AttackableStats stats) : stats(stats) {
			collision_radius = 50.0f;
		};

		virtual void Update(Client* game, float dt) {
			// TODO not so elegant
			Update(dt);
		};
		void Update(float dt);
		virtual void OnCollision(Client* game, IGameObject* other) override;
		virtual void Sync(std::vector<uint8_t>* data) override;
		void MoveToward(double x, double z);
		void StartMove(float x, float z);

		void TakeDamage(float damage, IGameObject* source);
		void Heal(float heal, IGameObject* source);

		void DoAction(GameObjectAction* newAction);
		virtual void Act(Client* game, float dt) override;

		virtual void Die();

		AttackableStats stats;
		AttackableStats modifiers;

		TargetType target_type;

		uint64_t prefab = UnitPrefab::GENERIC_EMPTY;

		Team team;

		basic_attack_info_t basic_attack_info = {
			BasicAttackType::MELEE, // type
			100, // range
			1, // damage
			1, // attack speed
			0.25, // hit point
			0, // attack started
			0, // since attack started
			0, // since attack
			500 // acquisition range;
		};

		spell_cast_info_t spell_cast_info = {
			0,
			-1,
		};

		std::vector<Buff*> buffs;
		NavigationGridAgent nav_agent;

	protected:
		// true -> initial spawn has been synced
		bool spawn_synced = false;

		// true -> stats need to be synced
		bool stats_updated = true;

	protected:
		GameObjectAction* current_action_ = nullptr;
	};
}
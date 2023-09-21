#pragma once

#include "pmg_types.h"
#include "pmg_physics.h"
#include "component_registry.h"
#include "navigation.h"

namespace PMG {
	class Game;
	class Buff;
	class Spell;
	class SpellTargetInfo;
	class GameObject;
	enum class EAnimation;

	enum class GameObjectActionType {
		STOP,
		MOVE,
		ATTACK_UNIT,
		CAST_SPELL,
	};

	class GameObjectAction {
	public:
		GameObjectAction(GameObjectActionType type) : type(type) {};
		GameObjectActionType type;
	};

	class GameObjectActionAttackUnit : public GameObjectAction {
	public:
		GameObjectActionAttackUnit(unsigned int target_net_id) : GameObjectAction(GameObjectActionType::ATTACK_UNIT), target_net_id(target_net_id) {};
		unsigned int target_net_id;
	};

	class GameObjectActionMove : public GameObjectAction {
	public:
		GameObjectActionMove(Physics::Vector3 target_point) : GameObjectAction(GameObjectActionType::MOVE), target_point(target_point) {};
		Physics::Vector3 target_point;
	};

	class GameObjectActionStop : public GameObjectAction {
	public:
		GameObjectActionStop() : GameObjectAction(GameObjectActionType::STOP) {};
	};

	class GameObjectActionCastSpell : public GameObjectAction {
	public:
		GameObjectActionCastSpell(int spell_index) : GameObjectAction(GameObjectActionType::CAST_SPELL), spell_index(spell_index) {};
		int spell_index;
		SpellTargetInfo* target_info = nullptr;
	};

	enum BasicAttackType {
		MELEE,
		RANGED,
	};

	typedef struct basic_attack_info {
		BasicAttackType type;
		int range;
		int damage;
		double attack_speed;
		double hit_point;
		unsigned long long last_attack;
		bool attack_started;
		unsigned long long attack_started_at;
	} basic_attack_info_t;

	typedef struct {
		unsigned long long cast_time;
		int current_spell;
	} spell_cast_info_t;

	typedef struct {
		bool can_move;
		int health;
		int max_health;
		int base_speed;
	} stats_t;

	class GameObjectController {
	public:
		virtual void Think(Game* game, GameObject* go) {};
	};

	extern int STATUS_STUNNED;

	/*
	class GameObject {
	public:
		UnitId unit_id;
		bool is_destroyed = false;

		GameObjectAction* current_action = nullptr;
		GameObjectController* controller = new GameObjectController();

		std::vector<Spell*> spells = {};

		basic_attack_info_t basic_attack_info = {
			BasicAttackType::RANGED,
			5,
			5,
			1,
			0.5,
			0
		};

		spell_cast_info_t spell_cast_info = {
			0,
			-1,
		};

		int current_status = 0;

		stats_t stats = {
			true,
			100,
			100,
			3
		};

		stats_t frame_stats = stats;

		Physics::Vector3 position;
		Physics::Vector3 rotation;
		double collision_radius = 0;
		Team team;
		nav_agent_t nav_agent;
		std::vector<Buff*> buffs;

		std::string current_animation;

		virtual void Update(float dt, Game* game);
		virtual void Think(float dt, Game* game);
		virtual bool IsTargetable() { return target_type != TargetType::UNTARGETABLE; };
		virtual Physics::Sphere GetHitbox() { return Physics::Sphere(position, 1); }
		void MoveToward(double x, double z, Game* game, double move_speed);
		void PlayAnimation(Game* game, std::string animation);


		void TakeDamage(Game* game, double damage, GameObject* source);

		virtual void OnCollision(Game* game, GameObject* other) {};


		virtual void Sync(std::vector<uint8_t>* data) {};
	};
	*/
}
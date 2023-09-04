#pragma once

#include "game_object.h"
#include "navigation.h"
#include "component_registry.h"
#include "pmg_types.h"
#include "spell.h"

namespace PMG {
	class Game;
	class GameObject;

	enum class CharacterActionType {
		STOP,
		MOVE,
		ATTACK_UNIT,
		CAST_SPELL,
	};

	class CharacterAction {
	public:
		CharacterAction(CharacterActionType type) : type(type) {};
		CharacterActionType type;
	};

	class CharacterActionAttackUnit : public CharacterAction {
	public:
		CharacterActionAttackUnit(unsigned int target_net_id) : CharacterAction(CharacterActionType::ATTACK_UNIT), target_net_id(target_net_id) {};
		unsigned int target_net_id;
	};

	class CharacterActionMove : public CharacterAction {
	public:
		CharacterActionMove(Physics::Vector3 target_point) : CharacterAction(CharacterActionType::MOVE), target_point(target_point) {};
		Physics::Vector3 target_point;
	};

	class CharacterActionStop : public CharacterAction {
	public:
		CharacterActionStop() : CharacterAction(CharacterActionType::STOP) {};
	};

	class CharacterActionCastSpell : public CharacterAction {
	public:
		CharacterActionCastSpell(int spell_index) : CharacterAction(CharacterActionType::CAST_SPELL), spell_index(spell_index) {};
		int spell_index;
		Physics::Vector3 target_point;
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

	class Character : public GameObject {
	public:
		void Think(float dt, Game* game);

		virtual bool IsTargetable() { return true; };
		virtual Physics::Sphere GetHitbox() { return Physics::Sphere(position, 1); }

		CharacterAction* current_action;
		nav_agent_t nav_agent;

		Team team;

		Physics::Vector3 position;
		Physics::Vector3 rotation;

		std::vector<Spell*> spells = { new TestSpell() };

		stats_t stats = {
			100,
			100,
			5
		};

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
	};
}
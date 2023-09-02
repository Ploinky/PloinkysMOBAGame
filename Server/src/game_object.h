#pragma once

#include "pmg_physics.h"
#include "component_registry.h"
#include "navigation.h"

namespace PMG {
	class Game;
	class GameObject;

	enum class GameObjectActionType {
		STOP,
		MOVE,
		ATTACK_UNIT,
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

	class GameObject {
	public:
		GameObjectAction* current_action;
		unsigned int unit_id;
		nav_agent_t nav_agent;

		Physics::Vector3 position;
		Physics::Vector3 rotation;

		stats_t stats = {
			100,
			100,
		};

		basic_attack_info_t basic_attack_info = {
			BasicAttackType::RANGED,
			5,
			5,
			1,
			0.5,
			0
		};
	};
}
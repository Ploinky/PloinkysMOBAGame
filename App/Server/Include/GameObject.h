#pragma once

#include "Common/pmg_types.h"
#include "Common/pmg_physics.h"
#include <Common/PMG_Common.h>

namespace PMG {
	class Client;
	class Buff;
	class Spell;
	class SpellTargetInfo;
	enum class EAnimation;
	class IGameObject;
	class Person;
	class Attackable;

	enum class GameObjectActionType {
		STOP,
		MOVE,
		ATTACK_UNIT,
		ATTACK_MOVE,
		CAST_SPELL,
	};

	class GameObjectAction {
	public:
		GameObjectAction(GameObjectActionType type, Client* game) : type(type), game_(game) {};
		GameObjectActionType type;

		virtual void Start() = 0;
		virtual void Do(float dt) = 0;
		virtual void Stop() = 0;

	protected:
		Client* game_;
	};

	class GameObjectActionAttackUnit : public GameObjectAction {
	public:
		GameObjectActionAttackUnit(UnitId self, UnitId target, Client* game) : GameObjectAction(GameObjectActionType::ATTACK_UNIT, game), selfId_(self), targetId_(target) {};

		virtual void Start() override;
		virtual void Do(float dt) override;
		virtual void Stop() override;

		UnitId targetId_;
	private:
		UnitId selfId_;
	};

	class GameObjectActionMove : public GameObjectAction {
	public:
		GameObjectActionMove(Person* person, Physics::Vector3 target_point, Client* game) : GameObjectAction(GameObjectActionType::MOVE, game), person_(person), target_point(target_point) {};
		Physics::Vector3 target_point;

		virtual void Start() override;
		virtual void Do(float dt) override;
		virtual void Stop() override;

	private:
		Person* person_;

		void StartMove(double x, double z);
		void MoveToward(double x, double z);
	};

	class GameObjectActionStop : public GameObjectAction {
	public:
		GameObjectActionStop(Attackable* self, Client* game) : GameObjectAction(GameObjectActionType::STOP, game), self_(self) {};

		virtual void Start() override;
		virtual void Do(float dt) override;
		virtual void Stop() override;

	private:
		Attackable* self_;
	};

	class GameObjectActionCastSpell : public GameObjectAction {
	public:
		GameObjectActionCastSpell(Person* person, int spell_index, Client* game) : GameObjectAction(GameObjectActionType::CAST_SPELL, game), person_(person), spell_index(spell_index) {};
		int spell_index;
		SpellTargetInfo* target_info = nullptr;

		virtual void Start() override;
		virtual void Do(float dt) override;
		virtual void Stop() override;

	private:
		Person* person_;
	};

	class GameObjectActionAttackMove : public GameObjectAction {
	public:
		GameObjectActionAttackMove(UnitId self, Physics::Vector3 targetPoint, Client* game) : GameObjectAction(GameObjectActionType::ATTACK_MOVE, game),
			selfId_(self), targetPoint_(targetPoint)
		{};

		virtual void Start() override;
		virtual void Do(float dt) override;
		virtual void Stop() override;

	private:
		UnitId selfId_;
		UnitId targetId_;
		Physics::Vector3 targetPoint_;
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
		bool attack_started;
		float sinceAttackStarted;

		float sinceAttack;

		int acquisitionRange;
	} basic_attack_info_t;

	typedef struct {
		// time since cast
		float cast_time;
		int current_spell;
	} spell_cast_info_t;

	class GameObjectController {
	public:
		virtual void Think(Client* game, IGameObject* go) {};
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
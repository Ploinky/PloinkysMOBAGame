#pragma once

#include "attackable.h"
#include "spell.h"

namespace PMG {
	class MissileSpellData {
	public:
		// aoe area
		double radius;

		// distance in units
		double distance = -1;

		// speed in units/sec
		double speed = 0;

		// can it?
		bool can_hit_self = false;

		// can it?
		bool can_hit_allies = false;

		// can it?
		bool can_hit_buildings = false;

		// -1 = infinite targets...
		int max_targets_hit = 1;
	};

	class MissileSpell : public IGameObject {
	public:
		MissileSpell(MissileSpellData data, Attackable* owner, SpellTargetInfo* target_info);

		Physics::Vector3 origin;
		SpellTargetInfo* target_info;
		Attackable* owner = nullptr;
		Attackable* target = nullptr;

		Team team;
		MissileSpellData spell_data;

		virtual void OnCollision(Game* game, IGameObject* other) override;
		virtual void TargetHit(Game* game, Attackable* owner, Attackable* target) = 0;
		virtual void Update(Game* game, float dt) override;
		virtual void Sync(std::vector<uint8_t>* data) override;

	private:
		bool spawn_synced = false;
		double time_since_tick_ = 0;
		double time_since_cast_ = 0;
	};
}
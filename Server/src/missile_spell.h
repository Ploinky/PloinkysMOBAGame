#pragma once

#include "game_object.h"

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

	class MissileSpell : public GameObject {
	public:
		MissileSpell(MissileSpellData data, GameObject* owner, SpellTargetInfo* target_info);

		Physics::Vector3 origin;
		SpellTargetInfo* target_info;
		GameObject* owner = nullptr;
		GameObject* target = nullptr;

		MissileSpellData spell_data;

		virtual void OnCollision(Game* game, GameObject* other);
		virtual void TargetHit(Game* game, GameObject* owner, GameObject* target) = 0;
		virtual void Update(float dt, Game* game);

	private:
		double time_since_tick_ = 0;
		double time_since_cast_ = 0;
	};
}
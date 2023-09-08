#pragma once

#include "game_object.h"
#include <set>

namespace PMG {
	class AreaSpellData {
	public:
		// aoe area
		double radius;

		// ticks per second
		double tickrate;

		// duration in ms
		double duration;

		// can it?
		bool can_hit_allies;

		// can it?
		bool can_hit_buildings;
	};

	class AreaSpell : public GameObject {
	public:
		AreaSpell(AreaSpellData data, SpellTargetInfo* target);

		SpellTargetInfo* target_info = nullptr;
		GameObject* owner = nullptr;

		AreaSpellData spell_data;
		std::set<GameObject*> objects_to_hit;

		virtual void OnCollision(Game* game, GameObject* other);
		virtual void TargetHit(Game* game, GameObject* owner, GameObject* target) = 0;
		virtual void Update(float dt, Game* game);

	private:
		double time_since_tick_ = 0;
		double time_since_cast_ = 0;
	};
}
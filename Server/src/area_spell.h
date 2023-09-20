#pragma once

#include "game_object.h"
#include <set>
#include "attackable.h"

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

	class AreaSpell : public IGameObject {
	public:
		AreaSpell(AreaSpellData data, SpellTargetInfo* target);

		SpellTargetInfo* target_info = nullptr;
		Attackable* owner = nullptr;

		Physics::Vector3 position;
		AreaSpellData spell_data;
		std::set<Attackable*> objects_to_hit;

		virtual void OnCollision(Game* game, IGameObject* other);
		virtual void TargetHit(Game* game, Attackable* owner, Attackable* target) = 0;
		virtual void Update(Game* game, float dt);

	private:
		double time_since_tick_ = 0;
		double time_since_cast_ = 0;
	};
}
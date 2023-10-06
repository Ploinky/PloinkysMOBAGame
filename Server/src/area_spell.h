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
		bool can_hit_self = false;

		// can it?
		bool can_hit_allies = false;

		// can it?
		bool can_hit_buildings = false;
	};

	class AreaSpell : public IGameObject {
	public:
		AreaSpell(AreaSpellData data, SpellTargetInfo* target);

		SpellTargetInfo* target_info = nullptr;
		Attackable* owner = nullptr;

		AreaSpellData spell_data;
		std::set<Attackable*> objects_to_hit;

		virtual void OnCollision(Client* game, IGameObject* other) override;
		virtual void TargetHit(Client* game, Attackable* owner, Attackable* target) = 0;
		virtual void Update(Client* game, float dt) override;
		virtual void Sync(std::vector<uint8_t>* data) override;

	private:
		double time_since_tick_ = 0;
		double time_since_cast_ = 0;
		// true -> initial spawn has been synced
		bool spawn_synced = false;

		// true -> stats need to be synced
		bool stats_updated = true;
	};
}
#pragma once

#include "attackable.h"
#include "spell.h"

namespace PMG {
	class Person : public Attackable {
	public:
		Person() : Attackable({
			100, 100
		}) {

			collision_radius = 1;
		};

		virtual void Update(Game* game, float dt) override;
		virtual void Act(Game* game, float dt) override;
		virtual void Sync(std::vector<uint8_t>* data) override;


		spell_cast_info_t spell_cast_info = {
			0,
			-1,
		};
		AttackableStats frame_stats;
		std::vector<Spell*> spells = {};
		int current_status = 0;
	};
}
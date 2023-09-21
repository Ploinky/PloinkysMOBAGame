#pragma once

#include "attackable.h"
#include "spell.h"

namespace PMG {
	class Person : public Attackable {
	public:
		Person() : Attackable({
			100, 100
		}) {};

		virtual void Update(Game* game, float dt) override;
		virtual void Act(Game* game, float dt) override;
		virtual void Sync(std::vector<uint8_t>* data) override;


		spell_cast_info_t spell_cast_info = {
			0,
			-1,
		};
		AttackableStats frame_stats;
		nav_agent_t nav_agent;
		std::vector<Spell*> spells = {};
		int current_status = 0;

	private:
		void MoveToward(double x, double z, Game* game, double move_speed);
	};
}
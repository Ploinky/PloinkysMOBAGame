#pragma once

#include "attackable.h"
#include "spell.h"

namespace PMG {
	class Person : public Attackable {
	public:
		Person() : Attackable({
			3, 100, 100
		}) {

			collision_radius = 1;
		};

		virtual void Update(Client* game, float dt) override;
		virtual void Act(Client* game, float dt) override;
		virtual void Sync(std::vector<uint8_t>* data) override;

		virtual void Die() override;


		spell_cast_info_t spell_cast_info = {
			0,
			-1,
		};
		
		std::vector<Spell*> spells = {};
		int current_status = 0;
	};
}
#pragma once

#include "Attackable.h"
#include "Spell.h"

namespace PMG {
	class Person : public Attackable {
	public:
		Person() : Attackable({
			300, 100, 100
		}) {
			isDead_ = false;
			respawnTimer_ = -1;
			collision_radius = 1;
		};

		virtual void Update(Client* game, float dt) override;
		virtual void Act(Client* game, float dt) override;
		virtual void Sync(std::vector<uint8_t>* data) override;

		virtual void Die() override;
		
		std::vector<Spell*> spells = {};
		int current_status = 0;

	private:
		bool isDead_;
		// current respawn timer in milliseconds
		int respawnTimer_;
	};
}
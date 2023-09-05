#pragma once

#include "character.h"
#include "spell.h"

namespace PMG {
	class FootballPerson : public Character {
	public:
		FootballPerson();
	};

	// Q
	class ThrowFootball : public Spell {
	public:
		ThrowFootball();
		virtual void OnCast(Game* game, Character* spell_owner, Physics::Vector3 target_point);
	};
}
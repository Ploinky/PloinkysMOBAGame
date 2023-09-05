#pragma once

#include "game_object.h"
#include "spell.h"

namespace PMG {
	class FootballPerson : public GameObject {
	public:
		FootballPerson();
	};

	// Q
	class ThrowFootball : public Spell {
	public:
		ThrowFootball();
		virtual void OnCast(Game* game, GameObject* spell_owner, Physics::Vector3 target_point);
	};
}
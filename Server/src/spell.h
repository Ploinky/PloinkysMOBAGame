#pragma once

#include "pmg_physics.h"
namespace PMG {
	class Character;
	class Game;

	enum class SpellType {
		MISSILE,
		TARGETED_MISSILE,
		TARGETED,
		AREA
	};

	class Spell {
	public:
		// point at which spell is cast in ms
		int cast_point = 100;
		int cooldown = 5000;
		int remaining_cooldown = -1;
		SpellType type;
		virtual void OnCast(Game* game, Character* spell_owner, Physics::Vector3 target_point) = 0;
	};
}
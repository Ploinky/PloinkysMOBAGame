#pragma once

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
		SpellType type;
		virtual void TargetHit(Game* game, Character* target) = 0;
	};

	class TestSpell : public Spell {
	public:
		TestSpell();
		virtual void TargetHit(Game* game, Character* target);
	};
}
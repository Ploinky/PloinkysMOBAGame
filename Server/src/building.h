#pragma once

#include "character.h"

namespace PMG {
	class Game;

	class Building : public Character {
	public:
		virtual void Think(float dt, Game* game);
	};
}
#pragma once

#include "game_object.h"

namespace PMG {
	class Game;

	class TowerGameObjectController : public GameObjectController {
	public:
		virtual void Think(Game* game, GameObject* go);
	};

	class Building : public GameObject {
	public:
		Building();
	};
}
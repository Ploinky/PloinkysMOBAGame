#pragma once

namespace PMG {
	class Game;

	class IGameObject {
	public:
		virtual void Update(Game* game, double dt) = 0;
		virtual void OnCollision(Game* game, IGameObject* other) = 0;
	};
}
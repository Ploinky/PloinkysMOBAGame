#pragma once

#include "igame_object.h"
#include <string>

namespace PMG {
	class ParticleSpawner : public IGameObject{
	public:
		virtual void Update(Game* game, float dt) override;

		// Synchronize with Clients...
		virtual void Sync(std::vector<uint8_t>* data) override; // TODO pure virtual

		// Something happened?!
		virtual void OnCollision(Game* game, IGameObject* other) override;

		std::string particle_name;
		IGameObject* attached_to = nullptr;
	};
}
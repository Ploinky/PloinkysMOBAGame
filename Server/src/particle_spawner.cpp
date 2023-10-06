#include "particle_spawner.h"
#include "pmg_networking.h"

namespace PMG {
	void ParticleSpawner::Update(Client* game, float dt) {};

	void ParticleSpawner::Sync(std::vector<uint8_t>* data) {
		Networking::PlayParticlePacket pck = Networking::PlayParticlePacket();
		pck.particle = particle_name;
		pck.unit = attached_to->unit_id;

		pck.Write(data);

		is_destroyed = true;
	};

	void ParticleSpawner::OnCollision(Client* game, IGameObject* other) {};
}
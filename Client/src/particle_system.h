#pragma once

#include "pmg_physics.h"
#include "particle.h"
#include <vector>
#include "renderer.h"
#include "game_object.h"
#include "particle_emitter.h"

namespace PMG {
	class ParticleSystem : public GameObject {
	public:
		ParticleSystem();
		virtual ~ParticleSystem() override;

		static ParticleSystem* Load(std::string file_name);

		void Render(Renderer* renderer);
		bool Initialize(Direct3D* direct3D);
		virtual void Update(double dt) override;

		void Attach(GameObject* other);

		void AddEmitter(ParticleEmitter* emitter);

	private:
		GameObject* attached_to_ = nullptr;
		std::vector<ParticleEmitter*> emitters_;
	};
}
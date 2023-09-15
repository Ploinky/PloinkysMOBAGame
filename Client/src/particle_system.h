#pragma once

#include "pmg_physics.h"
#include "particle.h"
#include <vector>
#include "renderer.h"

namespace PMG {
	class ParticleSystem {
	public:
		ParticleSystem();
		std::vector<Particle> particles;

		void Render(Renderer* renderer);
		bool Initialize(Direct3D* direct3D);
		void Update(double dt);

	private:
		ID3D11Buffer* vertex_buffer_;
		ID3D11Buffer* instance_buffer_;
		bool initialized;

		Physics::Vector3 position{};
		Physics::Vector3 rotation{};

		double life_time;
		double life;
	};
}
#pragma once

#include "pmg_physics.h"
#include "particle.h"
#include <vector>
#include "renderer.h"
#include "game_object.h"

namespace PMG {
	class ParticleSystem : public GameObject {
	public:
		ParticleSystem();
		std::vector<Particle> particles;

		void Render(Renderer* renderer);
		bool Initialize(Direct3D* direct3D);
		virtual void Update(double dt) override;

		void Attach(GameObject* other);

	private:
		ID3D11Buffer* vertex_buffer_;
		ID3D11Buffer* instance_buffer_;
		ID3D11ShaderResourceView* texture_;
		bool initialized;

		GameObject* attached_to_ = nullptr;

		double life_time;
		double life;
	};
}
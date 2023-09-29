#pragma once

#include "pmg_physics.h"
#include "particle.h"
#include "renderer.h"

namespace PMG {
	class ParticleEmitter {
	public:
		static ParticleEmitter* Load(std::string file_name);

		ParticleEmitter(std::string texture_name);
		~ParticleEmitter();
		void Update(float dt);

		void Render(Renderer* renderer);
		bool Initialize(Direct3D* direct3D);

		bool IsDone();

		Physics::Vector3 position;
		Physics::Vector3 rotation;
		Physics::Vector3 particle_angle;
		Physics::Vector3 particle_scale = { 1, 1, 1 };

		Physics::Vector3 particle_velocity;
		Physics::Vector3 particle_velocity_range;

		Physics::Vector3 particle_offset;
		Physics::Vector3 particle_offset_range;
		int particle_count;
		int emitter_lifetime;
		std::vector<Particle> particles;
		int emitter_life;
		bool static_angle = false;
	private:
		ID3D11Buffer* vertex_buffer_;
		ID3D11Buffer* instance_buffer_;
		ID3D11ShaderResourceView* texture_;
		bool initialized;
		std::string texture_name_;
	};
}
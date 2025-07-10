#pragma once

#include <string>
#include "common/pmg_physics.h"
#include "Particle.h"
#include <Direct3D.h>
#include <vector>
#include <list>

class ParticleEmitter {
public:
	static ParticleEmitter* Load(std::list<std::string> lines);

	ParticleEmitter(std::string texture_name);
	~ParticleEmitter();
	void Update(float dt);

	bool Initialize(Direct3D* direct3D);

	bool IsDone();

	Vector3 position;
	Vector3 rotation;
	Vector3 particle_angle;
	Vector3 particle_scale = { 1, 1, 1 };

	Vector3 particle_velocity;
	Vector3 particle_velocity_range;

	Vector3 particle_offset;
	Vector3 particle_offset_range;
	int particle_count;
	int emitter_lifetime;
	std::vector<Particle> particles;
	int emitter_life;
	bool static_angle = false;
	ID3D11Buffer* vertex_buffer_;
	ID3D11Buffer* instance_buffer_;
	bool initialized;
	std::string texture_name_;
};

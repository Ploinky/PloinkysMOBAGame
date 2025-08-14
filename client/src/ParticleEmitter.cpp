#include "ParticleEmitter.h"
#include "core/graphics/d3d11-graphics-engine.h"
#include "Camera.h"
#include <Common/PMG_Common.h>
#include <algorithm>

ParticleEmitter::ParticleEmitter(std::string texture_name) : texture_name_(texture_name) {
	emitter_life = 0;
};

ParticleEmitter* ParticleEmitter::Load(std::list<std::string> lines) {
	ParticleEmitter* particle_emitter = new ParticleEmitter("");

	for (std::string line : lines) {
		std::string token = line.substr(0, line.find("="));

		if(token.compare("texture_file") == 0) {
			particle_emitter->texture_name_ = line.substr(line.find("=") + 1);
		}
		else if (token.compare("particle_velocity_x") == 0) {
			particle_emitter->particle_velocity.x = std::stof(line.substr(line.find("=") + 1));
		}
		else if (token.compare("particle_velocity_y") == 0) {
			particle_emitter->particle_velocity.y = std::stof(line.substr(line.find("=") + 1));
		}
		else if (token.compare("particle_velocity_z") == 0) {
			particle_emitter->particle_velocity.z = std::stof(line.substr(line.find("=") + 1));
		}
		else if (token.compare("particle_velocity_range_x") == 0) {
			particle_emitter->particle_velocity_range.x = std::stof(line.substr(line.find("=") + 1));
		}
		else if (token.compare("particle_velocity_range_y") == 0) {
			particle_emitter->particle_velocity_range.y = std::stof(line.substr(line.find("=") + 1));
		}
		else if (token.compare("particle_velocity_range_z") == 0) {
			particle_emitter->particle_velocity_range.z = std::stof(line.substr(line.find("=") + 1));
		}
		else if (token.compare("particle_offset_x") == 0) {
			particle_emitter->particle_offset.x = std::stof(line.substr(line.find("=") + 1));
		}
		else if (token.compare("particle_offset_y") == 0) {
			particle_emitter->particle_offset.y = std::stof(line.substr(line.find("=") + 1));
		}
		else if (token.compare("particle_offset_z") == 0) {
			particle_emitter->particle_offset.z = std::stof(line.substr(line.find("=") + 1));
		}
		else if (token.compare("particle_offset_range_x") == 0) {
			particle_emitter->particle_offset_range.x = std::stof(line.substr(line.find("=") + 1));
		}
		else if (token.compare("particle_offset_range_y") == 0) {
			particle_emitter->particle_offset_range.y = std::stof(line.substr(line.find("=") + 1));
		}
		else if (token.compare("particle_offset_range_z") == 0) {
			particle_emitter->particle_offset_range.z = std::stof(line.substr(line.find("=") + 1));
		}
		else if (token.compare("particle_scale_x") == 0) {
			particle_emitter->particle_scale.x = std::stof(line.substr(line.find("=") + 1));
		}
		else if (token.compare("particle_scale_y") == 0) {
			particle_emitter->particle_scale.y = std::stof(line.substr(line.find("=") + 1));
		}
		else if (token.compare("particle_scale_z") == 0) {
			particle_emitter->particle_scale.z = std::stof(line.substr(line.find("=") + 1));
		}
		else if (token.compare("particle_count") == 0) {
			particle_emitter->particle_count = std::stoi(line.substr(line.find("=") + 1));
		}
		else if (token.compare("emitter_lifetime") == 0) {
			particle_emitter->emitter_lifetime = std::stoi(line.substr(line.find("=") + 1));
		}
		else if (token.compare("static_angle_x") == 0) {
			particle_emitter->particle_angle.x = std::stof(line.substr(line.find("=") + 1));
			particle_emitter->static_angle = true;
		}
		else if (token.compare("static_angle_y") == 0) {
			particle_emitter->particle_angle.y = std::stof(line.substr(line.find("=") + 1));
			particle_emitter->static_angle = true;
		}
		else if (token.compare("static_angle_z") == 0) {
			particle_emitter->particle_angle.z = std::stof(line.substr(line.find("=") + 1));
			particle_emitter->static_angle = true;
		}
	}

	return particle_emitter;
}

ParticleEmitter::~ParticleEmitter() {
}
static double test = 0;

void ParticleEmitter::Update(float dt) {
	for (auto particle = particles.begin(); particle != particles.end(); particle++) {
		particle->position.x += (dt / 1000.0) * particle->velocity.x;
		particle->position.y += (dt / 1000.0) * particle->velocity.y;
		particle->position.z += (dt / 1000.0) * particle->velocity.z;

		particle->lifetime += dt;

		// particle->velocity.y -= (9.81 * dt);
	}

	std::erase_if(particles, [](auto kv) { return kv.lifetime > 300; });

	test += dt;
	emitter_life += dt;

	if (emitter_life < emitter_lifetime && test > 0.2 && particles.size() < particle_count) {
		float velX = particle_velocity.x;

		if (particle_velocity_range.x != 0) {
			velX += ((rand() % (int)(particle_velocity_range.x * 100.0f)) / 100.0f) - particle_velocity_range.x / 2;

		}
		float velY = particle_velocity.y;

		if (particle_velocity_range.y != 0) {
			velY += ((rand() % (int)(particle_velocity_range.y * 100.0f)) / 100.0f) - particle_velocity_range.y / 2;

		}
		float velZ = particle_velocity.z;

		if (particle_velocity_range.z != 0) {
			velZ += ((rand() % (int)(particle_velocity_range.z * 100.0f)) / 100.0f) - particle_velocity_range.z / 2;

		}

		float posX = particle_offset.x;

		if (particle_offset_range.x != 0) {
			posX += ((rand() % (int)(particle_offset_range.x * 100.0f)) / 100.0f) - particle_offset_range.x / 2;

		}
		float posY = particle_offset.y;

		if (particle_offset_range.y != 0) {
			posY += ((rand() % (int)(particle_offset_range.y * 100.0f)) / 100.0f) - particle_offset_range.y / 2;

		}
		float posZ = particle_offset.z;

		if (particle_offset_range.z != 0) {
			posZ += ((rand() % (int)(particle_offset_range.z * 100.0f)) / 100.0f) - particle_offset_range.z / 2;

		}
		particles.push_back({ {posX, posY, posZ}, {1, 0, 0, 1}, { velX,velY, velZ } });
		//particles.push_back({ {0, 0, 0}, {1, 0, 0, 1}, { 0, (rand() % 100) / 10.0, 0 }, 0 });
		test = 0;
	}
}

bool ParticleEmitter::IsDone() {
	return emitter_life >= emitter_lifetime && particles.size() == 0;
}

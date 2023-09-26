#include "particle_system.h"
#include "direct3d.h"
#include "particle_shader.h"
#include <math.h>
#include "camera.h"
#include "DDSTextureLoader11.h"
#include "util.h"

namespace PMG {
	ParticleSystem::ParticleSystem() {
		has_healthbar = false;
		has_title = false;
	}

	ParticleSystem::~ParticleSystem() {
		for (ParticleEmitter* emitter : emitters_) {
			delete emitter;
		}
	}

	ParticleSystem* ParticleSystem::Load(std::string file_name) {
		std::list<std::string> lines = Util::ReadLinesFromFile(file_name);

		ParticleSystem* particle_system = new ParticleSystem();

		for (std::string line : lines) {
			ParticleEmitter* particle_emitter = ParticleEmitter::Load(line);
			particle_system->AddEmitter(particle_emitter);
		}

		return particle_system;
	}

	void ParticleSystem::Update(double dt) {
		if (attached_to_ != nullptr) {
			position = attached_to_->position;
			position.y += 1;
			// rotation = attached_to_->rotation;
		}

		bool done = false;

		// update emitters
		for (ParticleEmitter* emitter : emitters_) {
			emitter->position = position;
			emitter->rotation = rotation;

			emitter->Update(dt);

			if (emitter->IsDone()) {
				done = true;
			}
		}

		if (done) {
			destroy = true;
		}
	}

	void ParticleSystem::Attach(GameObject* other) {
		attached_to_ = other;
	}

	void ParticleSystem::AddEmitter(ParticleEmitter* emitter) {
		emitters_.push_back(emitter);
	}

	void ParticleSystem::Render(Renderer* renderer) {
		for (ParticleEmitter* emitter : emitters_) {
			emitter->Render(renderer);
		}
	}

	bool ParticleSystem::Initialize(Direct3D* direct3D) {
		for (ParticleEmitter* emitter : emitters_) {
			if (!emitter->Initialize(direct3D)) {
				return false;
			}
		}

		return true;
	}
}
#include "ParticleSystem.h"
#include "core/graphics/d3d11-graphics-engine.h"
#include <math.h>
#include "Camera.h"
#include <Common/PMG_Common.h>
#include "ParticleEmitter.h"

ParticleSystem::ParticleSystem() {
	has_healthbar = false;
	has_title = false;
}

ParticleSystem::~ParticleSystem() {
	for (ParticleEmitter* emitter : emitters_) {
		delete emitter;
	}
}

ParticleSystem* ParticleSystem::Load(std::string particleName, AssetManager* assetManager) {
	std::list<std::string> particleFilecontent = assetManager->LoadPlainFile(particleName);
	ParticleSystem* particle_system = new ParticleSystem();

	for (std::string line : particleFilecontent) {
		ParticleEmitter* particle_emitter = ParticleEmitter::Load(assetManager->LoadPlainFile(line));
		particle_system->AddEmitter(particle_emitter);
	}

	return particle_system;
}

void ParticleSystem::Update(float dt) {
	if (attached_to_ != nullptr) {
		position = attached_to_->position;
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

	position = other->position;
	for (ParticleEmitter* emitter : emitters_) {
		emitter->position = position;
	}
}

void ParticleSystem::AddEmitter(ParticleEmitter* emitter) {
	emitters_.push_back(emitter);
}

void ParticleSystem::Render(CRenderer* renderer) {
	for (ParticleEmitter* emitter : emitters_) {
		RenderCommand_t command {
			.eType = ERenderCommandType::SKINNED_MESH,
			.pModel = this
		};
		renderer->Submit(command);
	}
}

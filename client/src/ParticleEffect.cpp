#include "ParticleEffect.h"
#include "core/graphics/d3d11-graphics-engine.h"
#include <math.h>
#include "Camera.h"
#include <Common/PMG_Common.h>
#include "ParticleEmitter.h"

ParticleEffect::ParticleEffect() {
	has_healthbar = false;
	has_title = false;
}

ParticleEffect::~ParticleEffect() {
	for (ParticleEmitter* emitter : emitters_) {
		delete emitter;
	}
}

ParticleEffect* ParticleEffect::Load(std::string particleName, AssetManager* assetManager) {
	std::list<std::string> particleFilecontent = assetManager->LoadPlainFile(particleName);
	ParticleEffect* particle_system = new ParticleEffect();

	for (std::string line : particleFilecontent) {
		ParticleEmitter* particle_emitter = ParticleEmitter::Load(assetManager->LoadPlainFile(line));
		particle_system->AddEmitter(particle_emitter);
	}

	return particle_system;
}

void ParticleEffect::Update(float dt) {
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

void ParticleEffect::Attach(GameObject* other) {
	attached_to_ = other;
}

void ParticleEffect::AddEmitter(ParticleEmitter* emitter) {
	emitters_.push_back(emitter);
}

void ParticleEffect::Render(CRenderer* renderer) {
	for (ParticleEmitter* emitter : emitters_) {
		RenderCommand_t command {
			.eType = ERenderCommandType::SKINNED_MESH,
			.vec3Position = emitter->position,
			.vec3Rotation = emitter->rotation,
		};
		renderer->Submit(command);
	}
}

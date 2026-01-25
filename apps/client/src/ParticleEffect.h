#pragma once

#include "common/pmg_physics.h"
#include "Particle.h"
#include <vector>
#include "Renderer.h"
#include <common/PMG_Common.h>

class ParticleEmitter;

class ParticleEffect {
public:
	ParticleEffect();
	~ParticleEffect();

	static ParticleEffect* Load(std::string particleName, AssetManager* assetManager);

	void Render(CRenderer* renderer);
	void Update(float dt);

	void Attach(UnitId idOther);

	void AddEmitter(ParticleEmitter* emitter);
	std::vector<ParticleEmitter*> emitters_;

	Vector3 position; // TODO get this out of here
	Vector3 rotation; // TODO get this out of here
	UnitId attached_to_ = UNIT_ID_NONE;

	bool destroy = false;
};

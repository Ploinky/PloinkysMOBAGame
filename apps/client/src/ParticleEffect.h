#pragma once

#include "common/pmg_physics.h"
#include "Particle.h"
#include <vector>
#include "Renderer.h"
#include "GameObject.h"
#include <GameObject.h>
#include <common/PMG_Common.h>

class ParticleEmitter;

class ParticleEffect : public GameObject {
public:
	ParticleEffect();
	virtual ~ParticleEffect() override;

	static ParticleEffect* Load(std::string particleName, AssetManager* assetManager);

	void Render(CRenderer* renderer);
	virtual void Update(float dt) override;

	void Attach(GameObject* other);

	void AddEmitter(ParticleEmitter* emitter);
	std::vector<ParticleEmitter*> emitters_;

	Vector3 position; // TODO get this out of here
	Vector3 rotation; // TODO get this out of here
	GameObject* attached_to_ = nullptr;
};

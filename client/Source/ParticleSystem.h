#pragma once

#include "common/pmg_physics.h"
#include "Particle.h"
#include <vector>
#include "Renderer.h"
#include "GameObject.h"
#include "ParticleEmitter.h"
#include <GameObject.h>
#include <Common/PMG_Common.h>

class ParticleSystem : public GameObject {
public:
	ParticleSystem();
	virtual ~ParticleSystem() override;

	static ParticleSystem* Load(std::string particleName, AssetManager* assetManager);

	void Render(CRenderer* renderer);
	bool Initialize(Direct3D* direct3D);
	virtual void Update(float dt) override;

	void Attach(GameObject* other);

	void AddEmitter(ParticleEmitter* emitter);
	std::vector<ParticleEmitter*> emitters_;

private:
	GameObject* attached_to_ = nullptr;
};

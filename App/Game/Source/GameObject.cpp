#include "GameObject.h"
#include "Renderer.h"
#include <Common/PMG_Common.h>

namespace PMG {
	GameObject::~GameObject() {
	}

	void GameObject::Update(float dt) {
		m_animationComponent.Update(dt);
	}

	void GameObject::PlayAnimation(std::string animationId, bool loop) {
		m_animationComponent = CAnimationComponent(animationId, loop);
	}

	CAnimationComponent& GameObject::GetCurrentAnimation() {
		return m_animationComponent;
	}
}
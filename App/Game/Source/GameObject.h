#pragma once

#include "common/pmg_physics.h"
#include "common/pmg_types.h"
#include <string>

namespace PMG {
	class Mesh;
	class CRenderer;

	class CAnimationComponent {
	public:
		CAnimationComponent();
		CAnimationComponent(std::string strAnimationName, bool bLoop);

		void Update(float fTime);
		std::string GetAnimationName();
		float GetAnimationTime();
		bool DoLoop();

	private:
		std::string m_strAnimationName;
		bool m_bLoop;
		float m_fAnimationTime;
	};

	class GameObject {
	public:
		virtual ~GameObject();

		virtual void Update(float dt);

		virtual void PlayAnimation(std::string animationId, bool loop);
		CAnimationComponent& GetCurrentAnimation();

		UnitId unit_id;
		unsigned int health;
		unsigned int max_health;

		Physics::Vector3 position;
		Physics::Vector3 rotation;
		unsigned long long position_received;

		bool has_healthbar = true;
		bool has_title = true;

		Team team;

		std::string renderable = "";

		bool destroy = false;

	private:
		CAnimationComponent m_animationComponent;
	};
}
#pragma once

#include "common/pmg_physics.h"
#include "common/pmg_types.h"
#include <string>

class Mesh;
class CRenderer;

class CMovementComponent {
public:
	CMovementComponent();

	Vector3 GetTarget();
	void SetTarget(Vector3 vec3NewTarget);
	void ClearTarget();

	bool IsMoving();

private:
	Vector3 m_vec3Target;
	bool m_bIsMoving;
};

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
	CMovementComponent* GetMovementComponent();

	UnitId unit_id;
	unsigned int health;
	unsigned int max_health;

	Vector3 position;

	Vector3 rotation;
	unsigned long long position_received;

	bool has_healthbar = true;
	bool has_title = true;

	Team team;

	std::string renderable = "";

	bool destroy = false;

	bool dead = false;
	bool bIsCasting = false;
	bool bIsAttacking = false;

	uint64_t uPrefab = 0;
private:
	CAnimationComponent m_animationComponent;
	CMovementComponent m_movementComponent;
};

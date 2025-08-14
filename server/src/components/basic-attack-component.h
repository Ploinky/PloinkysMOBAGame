#pragma once

#include "GameObject.h"

enum class EAttackState {
    IDLE,
    APPROACHING,
    ATTACKING,
    BACKSWING,
    FINISHED,
    CANCELLED
};

typedef struct {
    UnitId idTarget;
	EAttackState eState;
	float fTimeInState;
} ActiveAttack_t;

class CBasicAttackComponent : public IComponent {
public:
    CBasicAttackComponent() {};
    
    float fRange = 100;
    float fAttackTime = 2000;
    float fAttackPoint = 1000;
	std::optional<ActiveAttack_t> optCurrentAttack;
};
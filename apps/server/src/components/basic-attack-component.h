#pragma once

#include <optional>

#include "common/pmg_types.h"

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

typedef struct {
    float fRange;
    float fAttackTime;
    float fAttackPoint;
	std::optional<ActiveAttack_t> optCurrentAttack;
}  BasicAttackComponent_t;
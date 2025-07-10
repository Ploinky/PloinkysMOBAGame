#pragma once

#include "Common/pmg_types.h"
#include "Common/pmg_physics.h"
#include <Common/PMG_Common.h>
#include "SpellTargetInfo.h"

enum class eGameObjectActionType {
	STOP,
	MOVE,
	ATTACK_UNIT,
	ATTACK_MOVE,
	CAST_SPELL,
	LINE_MISSILE
};

struct CGameState;
class CGameObject;

class CGameObjectAction {
public:
	CGameObjectAction(eGameObjectActionType type) : Type(type) {};
	virtual void Process(CGameState* pGameState, CGameObject* pGameObject, float fDelta) = 0;

protected:
	eGameObjectActionType Type;
};

class CGameObjectActionAttackUnit : public CGameObjectAction {
public:
	CGameObjectActionAttackUnit(UnitId idTarget) : CGameObjectAction(eGameObjectActionType::ATTACK_UNIT), IdTarget(idTarget) {};
	virtual void Process(CGameState* pGameState, CGameObject* pGameObject, float fDt) override;

private:
	UnitId IdTarget;
};

class CGameObjectActionMove : public CGameObjectAction {
public:
	CGameObjectActionMove(Vector3 vec3TargetPoint) : CGameObjectAction(eGameObjectActionType::MOVE), Vec3TargetPoint(vec3TargetPoint) {};
	virtual void Process(CGameState* pGameState, CGameObject* pGameObject, float fDt) override;

private:
	Vector3 Vec3TargetPoint;
};

class CGameObjectActionStop : public CGameObjectAction {
public:
	CGameObjectActionStop() : CGameObjectAction(eGameObjectActionType::STOP) {};
	virtual void Process(CGameState* pGameState, CGameObject* pGameObject, float fDt) override;
};

class CGameObjectActionCastSpell : public CGameObjectAction {
public:
	CGameObjectActionCastSpell(int iSpellIndex, SpellTargetInfo* pTargetInfo) : CGameObjectAction(eGameObjectActionType::CAST_SPELL),
		ISpellIndex(iSpellIndex), target_info(pTargetInfo) {};
	virtual void Process(CGameState* pGameState, CGameObject* pGameObject, float fDt) override;

private:
	int ISpellIndex;
	SpellTargetInfo* target_info = nullptr;
};

class CGameObjectActionAttackMove : public CGameObjectAction {
public:
	CGameObjectActionAttackMove(Vector3 targetPoint) : CGameObjectAction(eGameObjectActionType::ATTACK_MOVE), Vec3TargetPoint(targetPoint) {};
	virtual void Process(CGameState* pGameState, CGameObject* pGameObject, float fDt) override;

private:
	UnitId IdTarget;
	Vector3 Vec3TargetPoint;
};

class CGameObjectActionLineMissile : public CGameObjectAction {
public:
	CGameObjectActionLineMissile(Vector3 vec3TargetPoint) : CGameObjectAction(eGameObjectActionType::LINE_MISSILE), Vec3TargetPoint(vec3TargetPoint) {};
	virtual void Process(CGameState* pGameState, CGameObject* pGameObject, float fDt) override;

private:
	Vector3 Vec3TargetPoint;
};


typedef struct spell_cast_info {
	// time since cast
	float cast_time;
	int current_spell;
	UnitId IdTarget = UNIT_ID_NONE;
	Vector3 Vec3Target;
} spell_cast_info_t;

extern int STATUS_STUNNED;
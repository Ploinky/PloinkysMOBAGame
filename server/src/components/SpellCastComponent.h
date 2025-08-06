#pragma once

#include "GameObject.h"
#include "GameObject/Spell.h"
#include "SpellTargetInfo.h"
#include "components/Components.h"

enum class ESpellCastState {
	IDLE,
	APPROACHING,
	CASTING,
	CAST_POINT_REACHED,
	BACKSWING,
	FINISHED,
	CANCELLED
};

typedef struct SpellSlot_s {
	float fCooldownRemaining = 0.0f;
	ISpell* pSpell = nullptr;
} SpellSlot_t;

typedef struct ActiveCast_s {
	int nIndex = -1;
	ESpellCastState eState = ESpellCastState::IDLE;
	float fTimeInState = 0.0f;
	CSpellCastContext* spellCtx = nullptr;
} ActiveCast_t;

class CSpellCastComponent : public IComponent {
public:
	CSpellCastComponent(std::vector<SpellSlot_t> vecSpells);

	std::vector<SpellSlot_t> vecSpellSlots;
	std::optional<ActiveCast_t> optCurrentCast;
};
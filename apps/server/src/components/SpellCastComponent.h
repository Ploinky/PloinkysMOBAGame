#pragma once

#include "GameObject.h"
#include "GameObject/Spell.h"
#include "SpellTargetInfo.h"
#include "components/Components.h"
#include <optional>
#include <common/data/game-data.h>

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
	int level = 0;
	CAbilityData data;
} SpellSlot_t;

typedef struct ActiveCast_s {
	int nIndex = -1;
	ESpellCastState eState = ESpellCastState::IDLE;
	float fTimeInState = 0.0f;
	CSpellCastContext* spellCtx = nullptr;
} ActiveCast_t;

class CSpellCastComponent : public IComponent {
public:
	CSpellCastComponent() : CSpellCastComponent(std::vector<SpellSlot_t>()) {};
	CSpellCastComponent(std::vector<SpellSlot_t> vecSpells);

	std::vector<SpellSlot_t> vecSpellSlots;
	std::optional<ActiveCast_t> optCurrentCast;
};
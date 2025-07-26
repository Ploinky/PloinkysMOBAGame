#pragma once

#include "GameObject.h"
#include "GameObject/Spell.h"
#include "SpellTargetInfo.h"
#include "components/Components.h"

typedef struct SpellSlot_s {
	float fCooldownRemaining = 0.0f;
	bool bIsCasting = false;
	float fTimeSinceCast = 0.0f;
	ISpell* pSpell = nullptr;
	CSpellCastContext* spellCtx = nullptr;
} SpellSlot_t;

class CSpellCastComponent : public IComponent {
public:
	CSpellCastComponent(std::vector<SpellSlot_t> vecSpells);

	void CastSpell(CSpellCastContext* spellCtx);

	std::vector<SpellSlot_t>& GetSpellSlots();

private:
	std::vector<SpellSlot_t> m_vecSpells;
};
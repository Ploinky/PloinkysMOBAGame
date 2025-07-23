#include "GameObject.h"
#include "Spell.h"
#include "SpellTargetInfo.h"

struct SpellSlot_t {
	float fCooldownRemaining = 0.0f;
	bool bIsCasting = false;
	float fTimeSinceCast = 0.0f;
	ISpell* pSpell = nullptr;
	SpellTargetInfo* pTargetInfo;
};

class CSpellCastComponent : public IComponent {
public:
	CSpellCastComponent(std::vector<SpellSlot_t> vecSpells);
	virtual void Update(CGameState* pGameState, float fDelta) override;

	void CastSpell(int nIndex, SpellTargetInfo* pTargetInfo);

	std::vector<SpellSlot_t> GetSpellSlots();

private:
	std::vector<SpellSlot_t> m_vecSpells;
};
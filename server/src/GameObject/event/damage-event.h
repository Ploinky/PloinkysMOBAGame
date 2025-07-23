#include "../../GameState.h"

class CDamageCommand : public IGameCommand {
public:
    CDamageCommand(UnitId idSource, UnitId idTarget, int nDamage);

    virtual void Execute(CGameState* pGameState) override;

private:
    UnitId m_idSource;
    UnitId m_idTarget;
    int m_nDamage;
};
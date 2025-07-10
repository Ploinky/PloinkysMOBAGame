#pragma once

#include <IClientState.h>

class CConnectingState : public IClientState {
public:
	CConnectingState(IClientStateHandler* pHandler, int iWindowWidth, int iWindowHeight);
	virtual void Update(float fDelta) override;
	virtual void Render(CRenderer* pRenderer) override;
};

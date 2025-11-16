#include <ConnectingState.h>

CConnectingState::CConnectingState(IClientStateHandler* pHandler, int iWindowWidth, int iWindowHeight) : IClientState(pHandler, iWindowWidth, iWindowHeight) {
}

void CConnectingState::Update(float fDelta) {
		
}

void CConnectingState::Render(CRenderer* pRenderer) {
	pRenderer->RenderText(windowWidth_ / 2 - 100, windowHeight_ / 2 - 100, 200, 200, "Connecting...");
}

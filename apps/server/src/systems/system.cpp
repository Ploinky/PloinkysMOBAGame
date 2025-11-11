#include "system.h"

#include "GameState.h"

void ISystem::Process(CGameState* pGameState, IGameEvent* pGameEvent) {
    auto it = handlers.find(pGameEvent->GetType());
    if (it != handlers.end()) {
        for(auto evtHandler : it->second) {
            evtHandler(pGameState, pGameEvent);
        }
    }
}
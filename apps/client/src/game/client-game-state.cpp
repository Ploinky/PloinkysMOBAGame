#include "game/client-game-state.h"

void ISystem::Process(CClientGameState* pGameState, IGameEvent* pGameEvent) {
    auto it = handlers.find(pGameEvent->GetType());
    if (it != handlers.end()) {
        for(auto evtHandler : it->second) {
            evtHandler(pGameState, pGameEvent);
        }
    }
}
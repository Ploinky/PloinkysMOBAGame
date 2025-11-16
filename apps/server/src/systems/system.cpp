#include "system.h"

#include "game/server-game-state.h"

void ISystem::Process(CServerGameState* pGameState, IGameEvent* pGameEvent) {
    auto it = handlers.find(pGameEvent->GetType());
    if (it != handlers.end()) {
        for(auto evtHandler : it->second) {
            evtHandler(pGameState, pGameEvent);
        }
    }
}
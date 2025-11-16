#include "common/game/game-system.h"

#include "common/game/game-state.h"

void IGameSystem::Process(IGameState* pGameState, IGameEvent* pGameEvent) {
    auto it = handlers.find(pGameEvent->GetType());
    if (it != handlers.end()) {
        for(auto evtHandler : it->second) {
            evtHandler(pGameState, pGameEvent);
        }
    }
}

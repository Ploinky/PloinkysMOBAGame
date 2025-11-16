#pragma once

#include <unordered_map>
#include <functional>
#include <typeindex>

class IGameState;
class IGameEvent;

class IGameSystem {
public:
    virtual ~IGameSystem() = default;

    virtual void Update(IGameState* pGameState, float fDelta) {};
    void Process(IGameState* pGameState, IGameEvent* pGameEvent);
    virtual void Finalize(IGameState* pGameState) {};

protected:
    using HandlerFunc = std::function<void(IGameState*, IGameEvent*)>;
    std::unordered_map<std::type_index, std::vector<HandlerFunc>> handlers;

    template<typename TEvent>
    void RegisterHandler(std::function<void(IGameState*, TEvent*)> func) {
        if(!handlers.contains(typeid(TEvent))) {
            handlers.emplace(typeid(TEvent), std::vector<HandlerFunc>());
        }

        handlers[typeid(TEvent)].push_back([func](IGameState* state, IGameEvent* baseEvt) {
            func(state, static_cast<TEvent*>(baseEvt));
        });
    }
};
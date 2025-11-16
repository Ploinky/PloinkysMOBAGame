#pragma once

#include <unordered_map>
#include <functional>
#include <typeindex>

#define REGISTER_EVENT_HANDLER(EventType, Method) \
    RegisterHandler<EventType>([this](CGameState* state, EventType* e) { this->Method(state, e); });

class CGameState;
class IGameEvent;

class IGameSystem {
public:
    virtual ~IGameSystem() = default;

    virtual void Update(CGameState* pGameState, float fDelta) {};
    void Process(CGameState* pGameState, IGameEvent* pGameEvent);
    virtual void Finalize(CGameState* pGameState) {};

protected:
    using HandlerFunc = std::function<void(CGameState*, IGameEvent*)>;
    std::unordered_map<std::type_index, std::vector<HandlerFunc>> handlers;

    template<typename TEvent>
    void RegisterHandler(std::function<void(CGameState*, TEvent*)> func) {
        if(!handlers.contains(typeid(TEvent))) {
            handlers.emplace(typeid(TEvent), std::vector<HandlerFunc>());
        }

        handlers[typeid(TEvent)].push_back([func](CGameState* state, IGameEvent* baseEvt) {
            func(state, static_cast<TEvent*>(baseEvt));
        });
    }
};
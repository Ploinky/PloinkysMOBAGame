#pragma once

#include <unordered_map>
#include <functional>
#include <typeindex>


#ifndef REGISTER_EVENT_HANDLER
#define REGISTER_EVENT_HANDLER(EventType, Method) \
    RegisterHandler<EventType>([this](CClientGameState* state, EventType* e) { this->Method(state, e); });
#endif

class CClientGameState;
class IGameEvent;

class ISystem {
public:
    virtual ~ISystem() = default;

    virtual void Update(CClientGameState* pGameState, float fDelta) {};
    void Process(CClientGameState* pGameState, IGameEvent* pGameEvent);
    virtual void Finalize(CClientGameState* pGameState) {};

protected:
    using HandlerFunc = std::function<void(CClientGameState*, IGameEvent*)>;
    std::unordered_map<std::type_index, std::vector<HandlerFunc>> handlers;

    template<typename TEvent>
    void RegisterHandler(std::function<void(CClientGameState*, TEvent*)> func) {
        if(!handlers.contains(typeid(TEvent))) {
            handlers.emplace(typeid(TEvent), std::vector<HandlerFunc>());
        }

        handlers[typeid(TEvent)].push_back([func](CClientGameState* state, IGameEvent* baseEvt) {
            func(state, static_cast<TEvent*>(baseEvt));
        });
    }
};
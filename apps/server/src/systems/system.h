#pragma once

#include <unordered_map>
#include <functional>
#include <typeindex>

#ifndef REGISTER_EVENT_HANDLER
#define REGISTER_EVENT_HANDLER(EventType, Method) \
    RegisterHandler<EventType>([this](CServerGameState* state, EventType* e) { this->Method(state, e); });
#endif

class CServerGameState;
class IGameEvent;


class ISystem {
public:
    virtual ~ISystem() = default;

    virtual void Update(CServerGameState* pGameState, float fDelta) {};
    void Process(CServerGameState* pGameState, IGameEvent* pGameEvent);
    virtual void Finalize(CServerGameState* pGameState) {};

protected:
    using HandlerFunc = std::function<void(CServerGameState*, IGameEvent*)>;
    std::unordered_map<std::type_index, std::vector<HandlerFunc>> handlers;

    template<typename TEvent>
    void RegisterHandler(std::function<void(CServerGameState*, TEvent*)> func) {
        if(!handlers.contains(typeid(TEvent))) {
            handlers.emplace(typeid(TEvent), std::vector<HandlerFunc>());
        }

        handlers[typeid(TEvent)].push_back([func](CServerGameState* state, IGameEvent* baseEvt) {
            func(state, static_cast<TEvent*>(baseEvt));
        });
    }
};
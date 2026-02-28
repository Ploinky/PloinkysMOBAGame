#pragma once

#include <unordered_map>
#include <functional>
#include <typeindex>

#include "common/game/game-event.h"


template<typename TGameState>
class IGameState;

template<typename TOwner, typename IGameState, typename TEvent>
class CEventRegisterHelper {
public:
    using Method = std::function<void(IGameState*, TEvent*)>;
    CEventRegisterHelper(TOwner* pOwner, Method method) {
        pOwner->RegisterHandler(method);
    }
};

#ifndef REGISTER_EVENT_HANDLER
#define REGISTER_EVENT_HANDLER(HandlerClass, EventType, Method) \
    void Method(TGS*, EventType*); \
    CEventRegisterHelper<HandlerClass, TGS, EventType> reg__##Method = CEventRegisterHelper<HandlerClass, TGS, EventType>(this, [this](TGS* state, EventType* e) { this->Method(state, e); });
#endif

template<typename TGameState>
class IGameSystem {
public:
    virtual ~IGameSystem() = default;

    virtual void Update(TGameState* pGameState, float fDelta) {};
    void Process(TGameState* pGameState, IGameEvent* pGameEvent) {
        auto it = handlers.find(pGameEvent->GetType());
        if(it != handlers.end()) {
            for(HandlerFunc fn : it->second) {
                fn(pGameState, pGameEvent);
            }
        }
    };
    virtual void Finalize(TGameState* pGameState) {};

    using TGS = TGameState;
    using HandlerFunc = std::function<void(TGameState*, IGameEvent*)>;
    std::unordered_map<std::type_index, std::vector<HandlerFunc>> handlers;

    template<typename TEvent>
    void RegisterHandler(std::function<void(TGameState*, TEvent*)> func) {
        if(!handlers.contains(typeid(TEvent))) {
            handlers.emplace(typeid(TEvent), std::vector<HandlerFunc>());
        }

        handlers[typeid(TEvent)].push_back([func](TGameState* state, IGameEvent* baseEvt) {
            func(state, static_cast<TEvent*>(baseEvt));
        });
    }
};
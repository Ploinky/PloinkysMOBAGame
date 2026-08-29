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
    using Method = std::function<void(IGameState*, const TEvent&)>;
    CEventRegisterHelper(TOwner* pOwner, Method method) {
        pOwner->RegisterHandler(method);
    }
};

#ifndef REGISTER_EVENT_HANDLER
#define REGISTER_EVENT_HANDLER(HandlerClass, EventType, Method) \
    void Method(TGS*, const EventType&); \
    CEventRegisterHelper<HandlerClass, TGS, EventType> reg__##Method = CEventRegisterHelper<HandlerClass, TGS, EventType>(this, [this](TGS* state, const EventType& e) { this->Method(state, e); });
#endif

template<typename TGameState>
class IGameSystem {
public:
    virtual ~IGameSystem() = default;

    virtual void Update(TGameState* pGameState, float fDelta) {};
    void Process(TGameState* pGameState, const IGameEvent& gameEvent) {
        auto it = handlers.find(gameEvent.GetType());
        if(it != handlers.end()) {
            for(HandlerFunc fn : it->second) {
                fn(pGameState, gameEvent);
            }
        }
    };
    virtual void Finalize(TGameState* pGameState) {};

    using TGS = TGameState;
    using HandlerFunc = std::function<void(TGameState*, const IGameEvent&)>;
    std::unordered_map<std::type_index, std::vector<HandlerFunc>> handlers;

    template<typename TEvent>
    void RegisterHandler(std::function<void(TGameState*, const TEvent&)> func) {
        if(!handlers.contains(typeid(TEvent))) {
            handlers.emplace(typeid(TEvent), std::vector<HandlerFunc>());
        }

        handlers[typeid(TEvent)].push_back([func](TGameState* state, const IGameEvent& baseEvt) {
            func(state, static_cast<const TEvent&>(baseEvt));
        });
    }
};
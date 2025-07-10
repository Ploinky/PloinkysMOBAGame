#include "EventManager.h"

EventHandlerId CEventManager::m_currentHandlerId = 0;

std::unordered_map<EEventType, std::vector<std::pair<EventHandlerId, CEventManager::EventHandler>>>& CEventManager::GetSubscriptions() {
    static std::unordered_map<EEventType, std::vector<std::pair<EventHandlerId, EventHandler>>> subs;
    return subs;
}

EventHandlerId CEventManager::Subscribe(EEventType eType, EventHandler handler) {
    EventHandlerId id = m_currentHandlerId++;
    GetSubscriptions()[eType].push_back(std::pair(id, handler));
    return id;
}

void CEventManager::Unsubscribe(EEventType eType, EventHandlerId id) {
    auto& subs = GetSubscriptions();
    auto it = subs.find(eType);
    
    if (it == subs.end()) {
        return;
    }

    auto& vec = it->second;
    vec.erase(std::remove_if(vec.begin(), vec.end(), [id](const auto& pair) { return pair.first == id; }), vec.end());
}

void CEventManager::Emit(EEventType eType, void* data) {
    auto& subs = GetSubscriptions();
    auto it = subs.find(eType);
    if (it != subs.end()) {
        for (auto& handler : it->second) {
            handler.second(data);
        }
    }
}

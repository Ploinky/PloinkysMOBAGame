#pragma once

#include <functional>
#include "Common/pmg_types.h"

typedef uint64_t EventHandlerId;

struct SpellCastStartedData_t {
    UnitId unit;
    int nSpellSlot;
};

struct MoveIntentionData_t {
    UnitId unit;
    float x;
    float y;
};

enum class EEventType {
    SPELL_CAST_STARTED,
    MOVE_INTENTION
};

class CEventManager {
public:
using EventHandler = std::function<void(void*)>;

static EventHandlerId Subscribe(EEventType eType, EventHandler handler);
static void Unsubscribe(EEventType eType, EventHandlerId id);
static void Emit(EEventType eType, void* data);

private:
    static EventHandlerId m_currentHandlerId;
    static std::unordered_map<EEventType, std::vector<std::pair<EventHandlerId, EventHandler>>>& GetSubscriptions();
};
#ifndef _PMG_TYPES_H_
#define _PMG_TYPES_H_

#include <stdint.h>

class UnitPrefab {
public:
    static const uint64_t GENERIC_EMPTY = 0;
    static const uint64_t TOWER = 1;
    static const uint64_t FOOTBALL_PERSON = 2;
    static const uint64_t THROW_FOOTBALL = 3;
    static const uint64_t MINION = 4;
};

enum class Team {
    TEAM_1,
    TEAM_2,
    NEUTRAL
};

typedef uint64_t UnitId;

#define UNIT_ID_NONE UINT64_MAX

#endif
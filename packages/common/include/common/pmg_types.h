#ifndef _PMG_TYPES_H_
#define _PMG_TYPES_H_

#include <stdint.h>
#include <limits>

class UnitPrefab {
public:
    static const uint64_t GENERIC_EMPTY = 0;
    static const uint64_t TOWER = 1;
    static const uint64_t FOOTBALL_PERSON = 2;
    static const uint64_t THROW_FOOTBALL = 3;
    static const uint64_t MINION = 4;
    static const uint64_t STORMCALLER = 5;
};

enum class Team : uint8_t {
    TEAM_1,
    TEAM_2,
    NEUTRAL
};

typedef uint64_t UnitId;

typedef uint64_t ASSET_HANDLE;
constexpr ASSET_HANDLE INVALID_ASSET_HANDLE = std::numeric_limits<uint64_t>::max();

#define UNIT_ID_NONE UINT64_MAX

#endif
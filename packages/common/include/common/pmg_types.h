#ifndef _PMG_TYPES_H_
#define _PMG_TYPES_H_

#include <stdint.h>
#include <limits>

enum class Team : uint8_t {
    TEAM_1,
    TEAM_2,
    NEUTRAL
};

typedef uint64_t UnitId;

typedef uint64_t EngineHandle;
constexpr EngineHandle INVALID_HANDLE = std::numeric_limits<uint64_t>::max();


typedef EngineHandle ASSET_HANDLE;

#define UNIT_ID_NONE UINT64_MAX

#endif
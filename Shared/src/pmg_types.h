#pragma once

namespace PMG {
    class UnitPrefab {
    public:
        static const unsigned long GENERIC_EMPTY = 0;
        static const unsigned long TOWER = 1;
        static const unsigned long FOOTBALL_PERSON = 2;
        static const unsigned long THROW_FOOTBALL = 3;
        static const unsigned long MINION = 4;
    };

    enum class Team {
        TEAM_1,
        TEAM_2,
        NEUTRAL
    };

    typedef unsigned long UnitId;
}
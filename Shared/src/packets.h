#pragma once

namespace PMG {
    enum class PacketType {
        UNITSPAWN,
        UNITMOVE,
        UNITIDLE,
        UNITDESPAWN,
        GAME_TICK,
        PCK_NAME_REQUEST,
        CMD_READY,
        CMD_NOT_READY,
    };

    // ====== Server -> Client packets ======
    typedef struct pck_unit_move {
        unsigned int unit;
        float x;
        float y;
        float r;
    } pck_unit_move_t;

    typedef struct pck_unit_spawn {
        unsigned int unit;
        float x;
        float y;
    } pck_unit_spawn_t;

    typedef struct pck_unit_despawn_s {
        unsigned int unit;
    } pck_unit_despawn_t;

    // ====== Client -> Server commands ======
    typedef struct cmd_move {
        float nx;
        float ny;
    } cmd_move_t;

}
#pragma once

#include "pmg_types.h"
#include <vector>

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
        CMD_STOP,
        CMD_ATTACK,
        CMD_CAST,
        CMD_CAST_TARGET,
        PCK_CLIENT_UNIT_ID,
        PCK_STATS,
        PCK_SPELL_COOLDOWN,
        PCK_START_ANIMATION,
    };

    typedef struct {
        PacketType type;
        size_t size;
    } packet_header_t;

    typedef struct packet {
        packet_header_t header;
        std::vector<uint8_t> data;

        size_t size() const {
            return sizeof(packet_header_t) + data.size();
        }
    } packet_t;

    template<typename DataType>
    packet_t& operator << (packet_t& packet, const DataType& data) {
        static_assert(std::is_standard_layout<DataType>::value, "Data too complex for packet");

        size_t i = packet.data.size();

        packet.data.resize(packet.data.size() + sizeof(DataType));

        std::memcpy(packet.data.data() + i, &data, sizeof(DataType));

        packet.header.size = packet.size();

        return packet;
    }

    template<typename DataType>
    packet_t& operator >> (packet_t& packet, DataType& data) {
        static_assert(std::is_standard_layout<DataType>::value, "Data too complex for packet");

        size_t i = packet.data.size() - sizeof(DataType);

        std::memcpy(&data, packet.data.data() + i, sizeof(DataType));

        packet.data.resize(i);

        packet.header.size = packet.size();

        return packet;
    }
    
    inline packet_t& operator << (packet_t& packet, packet_t& data) {
        size_t i = packet.data.size();

        packet.data.resize(packet.data.size() + data.size());

        std::memcpy(packet.data.data() + i, &data.header, sizeof(packet_header_t));
        std::memcpy(packet.data.data() + i + sizeof(packet_header_t), data.data.data(), data.header.size - sizeof(packet_header_t));

        packet.header.size = packet.size();

        return packet;
    }

    inline packet_t& operator >> (packet_t& packet, packet_t& data) {
        // copy header from src packet data to dest packet header
        std::memcpy(&data.header, packet.data.data(), sizeof(packet_header_t));

        // resize dest packet to new size according to header
        data.data.resize(data.header.size - sizeof(packet_header_t));

        // copy data from src packet data to dest packet data, excluding the header that's already been copied
        std::memcpy(data.data.data(), packet.data.data() + sizeof(packet_header_t), data.header.size - sizeof(packet_header_t));

        // erase dest packet data from src packet
        packet.data.erase(packet.data.begin(), packet.data.begin() + data.header.size);

        // set new size in src packet header
        packet.header.size = packet.size();

        return packet;
    }

    // ====== Server -> Client packets ======
    typedef struct pck_unit_move {
        unsigned int unit;
        double x;
        double y;
        double z;
        double r;
    } pck_unit_move_t;

    typedef struct pck_unit_spawn {
        unsigned long unit;
        unsigned long unit_type;
        Team team;
        double x;
        double y;
    } pck_unit_spawn_t;

    typedef struct pck_unit_despawn_s {
        unsigned long unit;
    } pck_unit_despawn_t;

    typedef struct pck_unit_stats {
        unsigned int unit;
        int health;
        int max_health;
    } pck_unit_stats_t;
}
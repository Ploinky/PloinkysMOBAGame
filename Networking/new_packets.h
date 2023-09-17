#pragma once

#include <string>
#include "pmg_types.h"
#include <unordered_map>
#include <iostream>
#include <functional>
#include <stdint.h>

namespace PMG::Networking {
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
        PCK_PLAY_PARTICLE,
    };

    typedef struct {
        PacketType type;
        size_t size;
    } packet_header_t;

    template<typename PACKET_TYPE>
    class NetworkHandlerManager {
    public:
        std::unordered_map<PACKET_TYPE, std::function<void(std::vector<uint8_t>)>> packet_factory = {};

        void RegisterHandler(PACKET_TYPE type, std::function<void(std::vector<uint8_t>)> fn) {
            packet_factory.emplace(type, fn);
        }
        std::function<void(std::vector<uint8_t>)> GetHandler(PACKET_TYPE type) {
            if (packet_factory.find(type) != packet_factory.end()) {
                auto packet = packet_factory[type];
                return packet;
            }

            std::cout << "Unknown packet type" << std::endl;
            return nullptr;
        }
    };

    class BasePacket {
    public:
        BasePacket(PacketType type) : type(type) {};
        virtual void Read(std::vector<uint8_t>* data) = 0;
        virtual void Write(std::vector<uint8_t>* data) = 0;

        PacketType type;
    };

    class UnitIdPacket : public BasePacket {
    public:
        UnitIdPacket() : BasePacket(PacketType::PCK_CLIENT_UNIT_ID) {};
        virtual void Read(std::vector<uint8_t>* data) override;
        virtual void Write(std::vector<uint8_t>* data) override;

        UnitId unit_id;
    };

    class AnimationPacket : public BasePacket {
    public:
        AnimationPacket() : BasePacket(PacketType::PCK_START_ANIMATION) {};
        virtual void Read(std::vector<uint8_t>* data) override;
        virtual void Write(std::vector<uint8_t>* data) override;

        UnitId unit_id;
        std::string animation_name;
    };

    class GameTickPacket : public BasePacket {
    public:
        GameTickPacket() : BasePacket(PacketType::GAME_TICK) {};
        virtual void Read(std::vector<uint8_t>* data) override;
        virtual void Write(std::vector<uint8_t>* data) override;

        std::vector<uint8_t> data;
    };

    class MoveCommandPacket : public BasePacket {
    public:
        MoveCommandPacket() : BasePacket(PacketType::UNITMOVE) {};
        virtual void Read(std::vector<uint8_t>* data) override;
        virtual void Write(std::vector<uint8_t>* data) override;

        float x;
        float y;
    };

    class StopCommandPacket : public BasePacket {
    public:
        StopCommandPacket() : BasePacket(PacketType::CMD_STOP) {};
        virtual void Read(std::vector<uint8_t>* data) override;
        virtual void Write(std::vector<uint8_t>* data) override;
    };

    class AttackCommandPacket : public BasePacket {
    public:
        AttackCommandPacket() : BasePacket(PacketType::CMD_ATTACK) {};
        virtual void Read(std::vector<uint8_t>* data) override;
        virtual void Write(std::vector<uint8_t>* data) override;

        UnitId target_unit;
    };


    class CastTargetCommandPacket : public BasePacket {
    public:
        CastTargetCommandPacket() : BasePacket(PacketType::CMD_CAST_TARGET) {};
        virtual void Read(std::vector<uint8_t>* data) override;
        virtual void Write(std::vector<uint8_t>* data) override;

        int spell_slot;
        UnitId target;
    };

    class CastCommandPacket : public BasePacket {
    public:
        CastCommandPacket() : BasePacket(PacketType::CMD_CAST) {};
        virtual void Read(std::vector<uint8_t>* data) override;
        virtual void Write(std::vector<uint8_t>* data) override;

        int spell_slot;
        float x;
        float y;
        float z;
    };

    class CooldownPacket : public BasePacket {
    public:
        CooldownPacket() : BasePacket(PacketType::PCK_SPELL_COOLDOWN) {};
        virtual void Read(std::vector<uint8_t>* data) override;
        virtual void Write(std::vector<uint8_t>* data) override;

        UnitId unit;
        int spell_slot;
        int cooldown;
        int total_cooldown;
    };

    class UnitStatsPacket : public BasePacket {
    public:
        UnitStatsPacket() : BasePacket(PacketType::PCK_STATS) {};
        virtual void Read(std::vector<uint8_t>* data) override;
        virtual void Write(std::vector<uint8_t>* data) override;
        
        unsigned int unit;
        int health;
        int max_health;
    };

    class DespawnPacket : public BasePacket {
    public:
        DespawnPacket() : BasePacket(PacketType::UNITDESPAWN) {};
        virtual void Read(std::vector<uint8_t>* data) override;
        virtual void Write(std::vector<uint8_t>* data) override;

        unsigned long unit;
    };

    class SpawnPacket : public BasePacket {
    public:
        SpawnPacket() : BasePacket(PacketType::UNITSPAWN) {};
        virtual void Read(std::vector<uint8_t>* data) override;
        virtual void Write(std::vector<uint8_t>* data) override;

        unsigned long unit;
        unsigned long unit_type;
        Team team;
        float x;
        float y;
    };

    class UnitMovePacket : public BasePacket {
    public:
        UnitMovePacket() : BasePacket(PacketType::UNITMOVE) {};
        virtual void Read(std::vector<uint8_t>* data) override;
        virtual void Write(std::vector<uint8_t>* data) override;

        unsigned int unit;
        float x;
        float y;
        float z;
        float r;
    };

    class PlayParticlePacket : public BasePacket {
    public:
        PlayParticlePacket() : BasePacket(PacketType::PCK_PLAY_PARTICLE) {};
        virtual void Read(std::vector<uint8_t>* data) override;
        virtual void Write(std::vector<uint8_t>* data) override;

        unsigned int unit;
        std::string particle;
    };
}
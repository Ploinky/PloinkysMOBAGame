#pragma once

#include <string>
#include "pmg_types.h"
#include <unordered_map>
#include <iostream>
#include <functional>
#include <stdint.h>
#include "packets.h"

namespace PMG::Networking {
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

        double x;
        double y;
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
        double x;
        double y;
        double z;
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
}
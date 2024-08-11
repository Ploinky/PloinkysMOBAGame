#pragma once

#include <string>
#include "pmg_types.h"
#include <unordered_map>
#include <iostream>
#include <functional>
#include <stdint.h>
#include <cstring>

#include "Packets/PacketType.h"

#include "Packets/AttackCommand.h"
#include "Packets/AttackStartPacket.h"
#include "Packets/CastCommand.h"
#include "Packets/CastTargetCommand.h"
#include "Packets/CooldownPacket.h"
#include "Packets/DespawnPacket.h"
#include "Packets/GameTickPacket.h"
#include "Packets/LobbyGameStartPacket.h"
#include "Packets/LobbyReadyCommand.h"
#include "Packets/LobbySlotCommand.h"
#include "Packets/LobbySlotPacket.h"
#include "Packets/MoveCommand.h"
#include "Packets/PlayParticlePacket.h"
#include "Packets/SpawnPacket.h"
#include "Packets/StopCommand.h"
#include "Packets/UnitIdlePacket.h"
#include "Packets/UnitIdPacket.h"
#include "Packets/UnitMovePacket.h"
#include "Packets/UnitStatsPacket.h"

namespace PMG::Networking {
    template<typename PACKET_TYPE, typename HANDLER>
    class NetworkHandlerManager {
    public:
        std::unordered_map<PACKET_TYPE, HANDLER> packet_factory = {};

        void RegisterHandler(PACKET_TYPE type, HANDLER fn) {
            packet_factory.emplace(type, fn);
        }
        HANDLER GetHandler(PACKET_TYPE type) {
            if (packet_factory.find(type) != packet_factory.end()) {
                auto packet = packet_factory[type];
                return packet;
            }

            std::cout << "Unknown packet type" << std::endl;
            return nullptr;
        }
    };
}
#pragma once

#include <string>
#include "pmg_types.h"
#include <unordered_map>
#include <iostream>
#include <functional>
#include <stdint.h>
#include <cstring>

#include "packets/PacketType.h"

#include "packets/AttackCommand.h"
#include "packets/AttackStartPacket.h"
#include "packets/AttackFinishedPacket.h"
#include "packets/CastCommand.h"
#include "packets/CastTargetCommand.h"
#include "packets/CooldownPacket.h"
#include "packets/DespawnPacket.h"
#include "packets/GameTickPacket.h"
#include "packets/LobbyGameStartPacket.h"
#include "packets/LobbyPlayerLeftPacket.h"
#include "packets/LobbyReadyCommand.h"
#include "packets/LobbySlotCommand.h"
#include "packets/LobbySlotPacket.h"
#include "packets/MoveCommand.h"
#include "packets/PlayParticlePacket.h"
#include "packets/ServerInfoPacket.h"
#include "packets/SpawnPacket.h"
#include "packets/StopCommand.h"
#include "packets/UnitDeathPacket.h"
#include "packets/unit-respawn-packet.h"
#include "packets/UnitIdlePacket.h"
#include "packets/UnitIdPacket.h"
#include "packets/UnitMovePacket.h"
#include "packets/UnitMoveIntentionPacket.h"
#include "packets/UnitStatsPacket.h"
#include "packets/ScoreUpdatePacket.h"
#include "packets/SpellHitPacket.h"
#include "packets/SpellCastStartPacket.h"
#include "packets/characterselect/RemainingTimePacket.h"
#include "packets/GameEndPacket.h"

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
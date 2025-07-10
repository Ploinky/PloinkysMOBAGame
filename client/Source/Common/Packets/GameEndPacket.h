#pragma once

#include "BasePacket.h"
#include <Common/pmg_types.h>

class GameEndPacket : public BasePacket {
public:
	GameEndPacket() : BasePacket(PacketType::GAME_END_PACKET) {};
	virtual void Read(std::vector<uint8_t>* data) override;
	virtual void Write(std::vector<uint8_t>* data) override;

	bool victory;
};
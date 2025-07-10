#pragma once

#include "BasePacket.h"

class GameTickPacket : public BasePacket {
public:
	GameTickPacket() : BasePacket(PacketType::GAME_TICK) {};
	virtual void Read(std::vector<uint8_t>* data) override;
	virtual void Write(std::vector<uint8_t>* data) override;

	uint64_t tick;
};
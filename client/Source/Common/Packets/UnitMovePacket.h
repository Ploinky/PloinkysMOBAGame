#pragma once

#include "BasePacket.h"

class UnitMovePacket : public BasePacket {
public:
	UnitMovePacket() : BasePacket(PacketType::MOVE_CMD) {};
	virtual void Read(std::vector<uint8_t>* data) override;
	virtual void Write(std::vector<uint8_t>* data) override;

	unsigned int unit;
	float x;
	float y;
	float z;
	float r;
};
#pragma once

#include "BasePacket.h"

class MoveCommandPacket : public BasePacket {
public:
	MoveCommandPacket() : BasePacket(PacketType::MOVE_CMD) {};
	virtual void Read(std::vector<uint8_t>* data) override;
	virtual void Write(std::vector<uint8_t>* data) override;

	float x;
	float y;
};
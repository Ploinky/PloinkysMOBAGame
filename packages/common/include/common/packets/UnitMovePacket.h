#pragma once

#include "BasePacket.h"
#include "common/PMG_Common.h"

class UnitMovePacket : public BasePacket {
public:
	UnitMovePacket() : BasePacket(PacketType::UNITMOVE) {};
	virtual void Read(std::vector<uint8_t>* data) override;
	virtual void Write(std::vector<uint8_t>* data) const override;

	UnitId unit;
	float x;
	float y;
	float z;
	float r;
};
#pragma once

#include "BasePacket.h"

class UnitIdlePacket : public BasePacket {
public:
	UnitIdlePacket() : BasePacket(PacketType::UNITIDLE) {};
	virtual void Read(std::vector<uint8_t>* data) override;
	virtual void Write(std::vector<uint8_t>* data) const override;

	unsigned int unit;
	float x;
	float y;
	float z;
	float r;
};
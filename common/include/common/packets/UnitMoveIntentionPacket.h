#pragma once

#pragma once

#include "BasePacket.h"

class UnitMoveIntentionPacket : public BasePacket {
public:
    UnitMoveIntentionPacket() : BasePacket(PacketType::UNITMOVE_INTENTION) {};
	virtual void Read(std::vector<uint8_t>* data) override;
	virtual void Write(std::vector<uint8_t>* data) override;

	unsigned int unit;
	float x;
	float y;
	float z;
};
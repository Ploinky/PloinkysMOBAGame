#pragma once

#pragma once

#include "BasePacket.h"
#include "common/PMG_Common.h"

class UnitMoveIntentionPacket : public BasePacket {
public:
    UnitMoveIntentionPacket() : BasePacket(PacketType::UNITMOVE_INTENTION) {};
	virtual void Read(std::vector<uint8_t>* data) override;
	virtual void Write(std::vector<uint8_t>* data) const override;

	UnitId unit;
	float x;
	float y;
	float z;
};
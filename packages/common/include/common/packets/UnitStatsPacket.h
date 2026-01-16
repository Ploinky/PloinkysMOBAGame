#pragma once

#include "BasePacket.h"
#include "common/PMG_Common.h"

class UnitStatsPacket : public BasePacket {
public:
	UnitStatsPacket() : BasePacket(PacketType::PCK_STATS) {};
	virtual void Read(std::vector<uint8_t>* data) override;
	virtual void Write(std::vector<uint8_t>* data) const override;
	
	UnitId unit;
	uint64_t health;
	uint64_t max_health;
};
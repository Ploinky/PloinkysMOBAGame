#pragma once

#include "BasePacket.h"

class UnitStatsPacket : public BasePacket {
public:
	UnitStatsPacket() : BasePacket(PacketType::PCK_STATS) {};
	virtual void Read(std::vector<uint8_t>* data) override;
	virtual void Write(std::vector<uint8_t>* data) override;
	
	unsigned int unit;
	int health;
	int max_health;
};
#pragma once

#include "BasePacket.h"
#include "../pmg_types.h"

class SpawnPacket : public BasePacket {
public:
	SpawnPacket() : BasePacket(PacketType::UNITSPAWN) {};
	virtual void Read(std::vector<uint8_t>* data) override;
	virtual void Write(std::vector<uint8_t>* data) override;

	uint64_t unit;
	uint64_t unit_type;
	Team team;
	float x;
	float y;
	float z;
};
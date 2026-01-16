#pragma once

#include "BasePacket.h"
#include "../pmg_types.h"
#include <string>

class SpawnPacket : public BasePacket {
public:
	SpawnPacket() : BasePacket(PacketType::UNITSPAWN) {};
	virtual void Read(std::vector<uint8_t>* data) override;
	virtual void Write(std::vector<uint8_t>* data) const override;

	uint64_t unit;
	Team team;
	float x;
	float y;
	float z;
	std::string strEntId;
};
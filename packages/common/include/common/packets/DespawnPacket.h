#pragma once

#include "BasePacket.h"

class DespawnPacket : public BasePacket {
public:
	DespawnPacket() : BasePacket(PacketType::UNITDESPAWN) {};
	virtual void Read(std::vector<uint8_t>* data) override;
	virtual void Write(std::vector<uint8_t>* data) const override;

	uint64_t unit;
};
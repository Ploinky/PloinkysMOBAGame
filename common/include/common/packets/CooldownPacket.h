#pragma once

#include "BasePacket.h"
#include "../pmg_types.h"

class CooldownPacket : public BasePacket {
public:
	CooldownPacket() : BasePacket(PacketType::PCK_SPELL_COOLDOWN) {};
	virtual void Read(std::vector<uint8_t>* data) override;
	virtual void Write(std::vector<uint8_t>* data) override;

	UnitId unit;
	int spell_slot;
	int cooldown;
	int total_cooldown;
};
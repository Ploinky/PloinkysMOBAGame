#pragma once

#include "BasePacket.h"
#include "../pmg_types.h"
#include <string>

class SpellHitPacket : public BasePacket {
public:
	SpellHitPacket() : BasePacket(PacketType::PCK_SPELL_HIT) {
		unit = UNIT_ID_NONE;
		spell = "";
	};
	virtual void Read(std::vector<uint8_t>* data) override;
	virtual void Write(std::vector<uint8_t>* data) override;

	UnitId unit;
	std::string spell;
};
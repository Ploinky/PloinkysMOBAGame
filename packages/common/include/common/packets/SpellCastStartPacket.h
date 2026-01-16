#pragma once

#include "BasePacket.h"
#include "../pmg_types.h"

class SpellCastStartPacket : public BasePacket {
public:
	SpellCastStartPacket() : BasePacket(PacketType::PCK_SPELL_CAST_START) {
		unit = UNIT_ID_NONE;
	};
	virtual void Read(std::vector<uint8_t>* data) override;
	virtual void Write(std::vector<uint8_t>* data) const override;

	UnitId unit;
	UnitId idTarget;
};
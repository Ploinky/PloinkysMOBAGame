#pragma once

#include "BasePacket.h"
#include "../pmg_types.h"

class CUnitDeathPacket : public BasePacket {
public:
	CUnitDeathPacket() : BasePacket(PacketType::PCK_UNIT_DEATH) {
		idUnit = UNIT_ID_NONE;
	};

	virtual void Read(std::vector<uint8_t>* data) override;
	virtual void Write(std::vector<uint8_t>* data) const override;

	UnitId idUnit;
};
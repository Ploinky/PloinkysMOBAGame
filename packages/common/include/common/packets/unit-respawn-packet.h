#pragma once

#include "BasePacket.h"
#include "../pmg_types.h"

class CUnitRespawnPacket : public BasePacket {
public:
	CUnitRespawnPacket() : BasePacket(PacketType::PCK_UNIT_RESPAWN) {
		idUnit = UNIT_ID_NONE;
	};

	virtual void Read(std::vector<uint8_t>* data) override;
	virtual void Write(std::vector<uint8_t>* data) const override;

	UnitId idUnit;
};
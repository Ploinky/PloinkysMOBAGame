#pragma once

#include "BasePacket.h"
#include "../pmg_types.h"

class CPickedUpEntityPacket : public BasePacket {
public:
	CPickedUpEntityPacket() : BasePacket(PacketType::PCK_PICKED_UP_ENTITY) {
		idUnit = UNIT_ID_NONE;
	};

	virtual void Read(std::vector<uint8_t>* data) override;
	virtual void Write(std::vector<uint8_t>* data) const override;

	UnitId idUnit;
    UnitId idPickedUpUnit;
};
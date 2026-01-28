#pragma once

#include "BasePacket.h"
#include "../pmg_types.h"

class CPickUpEntityCommand : public BasePacket {
public:
	CPickUpEntityCommand() : BasePacket(PacketType::CMD_PICK_UP_ENTITY) {
		idUnit = UNIT_ID_NONE;
	};

	virtual void Read(std::vector<uint8_t>* data) override;
	virtual void Write(std::vector<uint8_t>* data) const override;

	UnitId idUnit;
};
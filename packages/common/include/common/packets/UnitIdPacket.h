#pragma once

#include "BasePacket.h"
#include "../pmg_types.h"

class UnitIdPacket : public BasePacket {
public:
	UnitIdPacket() : BasePacket(PacketType::PCK_CLIENT_UNIT_ID) {};
	virtual void Read(std::vector<uint8_t>* data) override;
	virtual void Write(std::vector<uint8_t>* data) override;

	UnitId unit_id;
};
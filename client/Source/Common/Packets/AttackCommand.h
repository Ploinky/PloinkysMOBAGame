#pragma once

#include <Common/pmg_types.h>
#include "BasePacket.h"

class AttackCommandPacket : public BasePacket {
public:
	AttackCommandPacket() : BasePacket(PacketType::CMD_ATTACK) {};
	virtual void Read(std::vector<uint8_t>* data) override;
	virtual void Write(std::vector<uint8_t>* data) override;

	UnitId target_unit;
};
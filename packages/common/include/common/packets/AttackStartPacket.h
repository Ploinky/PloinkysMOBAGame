#pragma once

#include "BasePacket.h"
#include "../pmg_types.h"

typedef struct attack_start_pck {
	UnitId unit;
	UnitId target;
} attack_start_pck_t;

class AttackStartPacket : public BasePacket {
public:
	AttackStartPacket() : BasePacket(PacketType::PCK_ATTACK_START) {};
	virtual void Read(std::vector<uint8_t>* data) override;
	virtual void Write(std::vector<uint8_t>* data) override;

	attack_start_pck_t content;
};
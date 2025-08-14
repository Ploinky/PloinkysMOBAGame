#pragma once

#include "BasePacket.h"
#include "../pmg_types.h"

typedef struct attack_finished_pck_t {
	UnitId unit;
} attack_finished_pck_t;

class CAttackFinishedPacket : public BasePacket {
public:
	CAttackFinishedPacket() : BasePacket(PacketType::PCK_ATTACK_FINISHED) {};
	virtual void Read(std::vector<uint8_t>* data) override;
	virtual void Write(std::vector<uint8_t>* data) override;

	attack_finished_pck_t content;
};
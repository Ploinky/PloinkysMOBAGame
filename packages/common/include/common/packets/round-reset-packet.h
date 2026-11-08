#pragma once

#include "BasePacket.h"
#include "../pmg_types.h"

class CRoundResetPacket : public BasePacket {
public:
	CRoundResetPacket() : BasePacket(PacketType::ROUND_RESET_PACKET) {};
	virtual void Read(std::vector<uint8_t>* data) override;
	virtual void Write(std::vector<uint8_t>* data) const override;
};
#pragma once

#include "../BasePacket.h"
#include "../../pmg_types.h"
#include <memory.h>

class CRemainingTimePacket : public BasePacket {
public:
	CRemainingTimePacket() : BasePacket(PacketType::CHARACER_SELECT_REMAINING_TIME) {};
	virtual void Read(std::vector<uint8_t>* data) override;
	virtual void Write(std::vector<uint8_t>* data) override;

	int TimeInMS;
};
#pragma once

#include "BasePacket.h"
#include "../pmg_types.h"

class CRoundEndPacket : public BasePacket {
public:
	CRoundEndPacket() : BasePacket(PacketType::ROUND_END_PACKET) {};
	CRoundEndPacket(Team eWinner) : eWinner(eWinner), BasePacket(PacketType::ROUND_END_PACKET) {};
	virtual void Read(std::vector<uint8_t>* data) override;
	virtual void Write(std::vector<uint8_t>* data) const override;

	Team eWinner;
};
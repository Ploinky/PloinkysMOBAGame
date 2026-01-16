#pragma once

#include "BasePacket.h"
#include "../pmg_types.h"

class ScoreUpdatePacket : public BasePacket {
public:
	ScoreUpdatePacket() : BasePacket(PacketType::SCORE_UPDATE_PACKET) {};
	virtual void Read(std::vector<uint8_t>* data) override;
	virtual void Write(std::vector<uint8_t>* data) const override;

	uint16_t usTeam1Score;
	uint16_t usTeam2Score;
};
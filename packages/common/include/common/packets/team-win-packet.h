#pragma once

#include "BasePacket.h"
#include "../pmg_types.h"

class CTeamWinPacket : public BasePacket {
public:
	CTeamWinPacket() : BasePacket(PacketType::PCK_TEAM_WIN), eTeam(Team::NEUTRAL) {};
	CTeamWinPacket(Team eTeam) : BasePacket(PacketType::PCK_TEAM_WIN), eTeam(eTeam) {};

	virtual void Read(std::vector<uint8_t>* data) override;
	virtual void Write(std::vector<uint8_t>* data) const override;

	Team eTeam;
};
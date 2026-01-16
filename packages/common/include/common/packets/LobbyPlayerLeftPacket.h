#pragma once

#include "BasePacket.h"

class LobbyPlayerLeftPacket : public BasePacket {
public:
	LobbyPlayerLeftPacket() : BasePacket(PacketType::PCK_LOBBY_PLAYER_LEFT) {};
	virtual void Read(std::vector<uint8_t>* data) override;
	virtual void Write(std::vector<uint8_t>* data) const override;

	uint64_t uPlayerId;
};
#pragma once

#include "BasePacket.h"

class LobbySlotPacket : public BasePacket {
public:
	LobbySlotPacket() : BasePacket(PacketType::LOBBY_PLAYER_PCK) {};
	virtual void Read(std::vector<uint8_t>* data) override;
	virtual void Write(std::vector<uint8_t>* data) const override;

	uint8_t slot;
	uint64_t steamId;
	bool isReady;
};
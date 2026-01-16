#pragma once

#include "BasePacket.h"

class LobbySlotCmd : public BasePacket {
public:
	LobbySlotCmd() : BasePacket(PacketType::LOBBY_CMD_SLOT) {};
	virtual void Read(std::vector<uint8_t>* data) override;
	virtual void Write(std::vector<uint8_t>* data) const override;

	uint8_t slot;
};
#pragma once

#include "BasePacket.h"

class LobbyReadyCmd : public BasePacket {
public:
	LobbyReadyCmd() : BasePacket(PacketType::LOBBY_CMD_READY) {};
	virtual void Read(std::vector<uint8_t>* data) override;
	virtual void Write(std::vector<uint8_t>* data) override;
};
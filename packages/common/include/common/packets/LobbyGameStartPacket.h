#pragma once

#include "BasePacket.h"

class LobbyGameStartPck : public BasePacket {
public:
	LobbyGameStartPck() : BasePacket(PacketType::LOBBY_GAME_START) {};

	virtual void Read(std::vector<uint8_t>* data) override;
	virtual void Write(std::vector<uint8_t>* data) const override;
};
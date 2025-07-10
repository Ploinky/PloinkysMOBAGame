#pragma once

#include "BasePacket.h"
#include <Common/pmg_types.h>

class ServerInfoPacket : public BasePacket {
public:
	ServerInfoPacket() : BasePacket(PacketType::SERVER_INFO) {};
	virtual void Read(std::vector<uint8_t>* data) override;
	virtual void Write(std::vector<uint8_t>* data) override;

	uint8_t ubPlayerCount;
	uint8_t ubPlayerMaxCount;
	uint8_t ubNameLen;
	char* szName;
};
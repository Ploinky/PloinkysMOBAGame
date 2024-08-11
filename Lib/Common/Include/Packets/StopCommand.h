#pragma once

#include "BasePacket.h"

namespace PMG::Networking {
	class StopCommandPacket : public BasePacket {
	public:
		StopCommandPacket() : BasePacket(PacketType::CMD_STOP) {};
		virtual void Read(std::vector<uint8_t>* data) override;
		virtual void Write(std::vector<uint8_t>* data) override;
	};
}
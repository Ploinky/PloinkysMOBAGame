#pragma once

#include "BasePacket.h"

namespace PMG::Networking {
	class MoveCommandPacket : public BasePacket {
	public:
		MoveCommandPacket() : BasePacket(PacketType::UNITMOVE) {};
		virtual void Read(std::vector<uint8_t>* data) override;
		virtual void Write(std::vector<uint8_t>* data) override;

		float x;
		float y;
	};
}
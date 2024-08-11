#pragma once

#include "BasePacket.h"

namespace PMG::Networking {
	class CastCommandPacket : public BasePacket {
	public:
		CastCommandPacket() : BasePacket(PacketType::CMD_CAST) {};
		virtual void Read(std::vector<uint8_t>* data) override;
		virtual void Write(std::vector<uint8_t>* data) override;

		int spell_slot;
		float x;
		float y;
		float z;
	};
}
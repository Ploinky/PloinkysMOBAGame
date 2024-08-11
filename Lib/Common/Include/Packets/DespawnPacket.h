#pragma once

#include "BasePacket.h"

namespace PMG::Networking {
	class DespawnPacket : public BasePacket {
	public:
		DespawnPacket() : BasePacket(PacketType::UNITDESPAWN) {};
		virtual void Read(std::vector<uint8_t>* data) override;
		virtual void Write(std::vector<uint8_t>* data) override;

		uint64_t unit;
	};
}
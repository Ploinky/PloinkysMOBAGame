#pragma once

#include "BasePacket.h"
#include <Common/pmg_types.h>

namespace PMG::Networking {
	class UnitIdPacket : public BasePacket {
	public:
		UnitIdPacket() : BasePacket(PacketType::PCK_CLIENT_UNIT_ID) {};
		virtual void Read(std::vector<uint8_t>* data) override;
		virtual void Write(std::vector<uint8_t>* data) override;

		UnitId unit_id;
	};
}
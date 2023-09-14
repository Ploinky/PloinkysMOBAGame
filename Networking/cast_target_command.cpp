#include "new_packets.h"

namespace PMG::Networking {
	void CastTargetCommandPacket::Read(std::vector<uint8_t>* data) {
		std::memcpy(&spell_slot, data->data() + sizeof(packet_header_t), sizeof(spell_slot));
		std::memcpy(&target, data->data() + sizeof(packet_header_t) + sizeof(spell_slot), sizeof(target));
	}

	void CastTargetCommandPacket::Write(std::vector<uint8_t>* data) {
		packet_header_t header{};
		header.type = type;
		header.size = sizeof(packet_header_t) + sizeof(spell_slot) + sizeof(target);

		data->resize(header.size);

		std::memcpy(data->data(), &header, sizeof(packet_header_t));
		std::memcpy(data->data() + sizeof(packet_header_t), &spell_slot, sizeof(spell_slot));
		std::memcpy(data->data() + sizeof(packet_header_t) + sizeof(spell_slot), &target, sizeof(target));
	}
}